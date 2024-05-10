#include <bits/stdc++.h>
using namespace std;
const int SIZE = 4; // سایز مربع جادویی

// مشخصات هر کروموزوم
struct Individual
{
    vector<vector<int>> square;
    int fitness;
};

// درست کردن ی کروموزوم رندم
vector<vector<int>> generate_random_square()
{
    vector<vector<int>> square(SIZE, vector<int>(SIZE));

    vector<int> numbers(SIZE * SIZE);
    for (int i = 0; i < SIZE * SIZE; ++i)
    {
        numbers[i] = i + 1;
    }

    random_device rd;
    mt19937 g(rd());
    shuffle(numbers.begin(), numbers.end(), g);

    int index = 0;
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            square[i][j] = numbers[index++];
        }
    }

    return square;
}

// حساب کردن فیتنس برای یک کروموزوم
int calculate_fitness(const vector<vector<int>> &square)
{
    int target_sum = SIZE * (SIZE * SIZE + 1) / 2;
    int fitness = 0;

    // حساب کردن محموع سطر
    for (int i = 0; i < SIZE; ++i)
    {
        int rowSum = 0;
        for (int j = 0; j < SIZE; ++j)
        {
            rowSum += square[i][j];
        }
        fitness += abs(rowSum - target_sum);
    }

    // حساب کردن محموع ستون
    for (int j = 0; j < SIZE; ++j)
    {
        int colSum = 0;
        for (int i = 0; i < SIZE; ++i)
        {
            colSum += square[i][j];
        }
        fitness += abs(colSum - target_sum);
    }

    // حساب کردن محموع قطر ها
    int diagonal_sum1 = 0;
    int diagonal_sum2 = 0;
    for (int i = 0; i < SIZE; ++i)
    {
        diagonal_sum1 += square[i][i];
        diagonal_sum2 += square[i][SIZE - 1 - i];
    }
    fitness += abs(diagonal_sum1 - target_sum);
    fitness += abs(diagonal_sum2 - target_sum);

    return fitness;
}

// crossover تابع
Individual crossover(const Individual &parent1, const Individual &parent2)
{
    Individual child;
    child.square = parent1.square;

    // انتخاب یک ردیف و ستون برای ترکیب یا همان crossover
    int row = rand() % SIZE;
    int col = rand() % SIZE;

    // crossover انجام
    for (int i = 0; i < SIZE; ++i)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            if (i == row || j == col)
            {
                child.square[i][j] = parent2.square[i][j];
            }
        }
    }

    // آپدیت کردن فیتنس مربوط به فرزند ایجاد شده
    child.fitness = calculate_fitness(child.square);

    return child;
}

// mutation تابع
void mutate(Individual &individual)
{
    // پیدا کردن یک خانه رندوم تا روی ان جهش انجام شود
    int row = rand() % SIZE;
    int col = rand() % SIZE;

    // انتخاب عدد برای قرار گرفتن درون ژن
    int number = rand() % (SIZE * SIZE) + 1;

    // انجام جهش
    individual.square[row][col] = number;

    // آپدیت کردن فیتنس کروموزوم
    individual.fitness = calculate_fitness(individual.square);
}

// ساخت جمعیت اولیه
vector<Individual> create_initial_population(int populationSize)
{
    vector<Individual> population(populationSize);

    for (int i = 0; i < populationSize; ++i)
    {
        population[i].square = generate_random_square();
        population[i].fitness = calculate_fitness(population[i].square);
    }

    return population;
}

// انتخاب بهترین های جامعه با توجه به حدود تعریف شده
vector<Individual> selection(const vector<Individual> &population, int elite_size)
{
    vector<Individual> sorted_population = population;
    sort(sorted_population.begin(), sorted_population.end(), [](const Individual &a, const Individual &b)
         { return a.fitness < b.fitness; });

    vector<Individual> selected_population(sorted_population.begin(), sorted_population.begin() + elite_size);

    return selected_population;
}

// تولید نسل بعد با تعداد خواسته شده
vector<Individual> generate_next_generation(const vector<Individual> &population, int elite_size, int offspringSize)
{
    vector<Individual> next_generation;

    // انتخاب کردن بهترین های یک حامعه کروموزومی
    vector<Individual> selected_population = selection(population, elite_size);

    // اضافه کردن بهترین ها به نسل بعدی
    next_generation.insert(next_generation.end(), selected_population.begin(), selected_population.end());

    // انجام دادن ترکیب و جهش بر روی جمعیت جدید (بجز بهترین های قبلی)
    while (next_generation.size() < offspringSize)
    {
        // پیدا کردن دو والد رندوم از بهترین های نسل قبلی
        int parent1Index = rand() % selected_population.size();
        int parent2Index = rand() % selected_population.size();

        // crossover انجام
        Individual child = crossover(selected_population[parent1Index], selected_population[parent2Index]);

        // mutation  انحام
        mutate(child);

        // اضافه کردن فرزند های جدید به بهترین های نسل قبل به عنوان نسل جدید
        next_generation.push_back(child);
    }

    return next_generation;
}

// پیدا کردن بهترین کروموزوم
Individual find_best_individual(const vector<Individual> &population)
{
    Individual best_individual = population[0];

    for (const Individual &individual : population)
    {
        if (individual.fitness < best_individual.fitness)
        {
            best_individual = individual;
        }
    }

    return best_individual;
}

bool is_in_answers(const Individual &best_individual, const vector<Individual> &answers)
{
    for (int i = 0; i < answers.size(); i++)
    {
        if (best_individual.square == answers[i].square)
        {
            return true;
        }
    }
    return false;
}

void print_square_answers(const vector<Individual> &answers)
{
    ofstream fw("answers.txt", ofstream::out);
    if (fw.is_open())
    {
        for (int i = 0; i < answers.size(); i++)
        {
            fw << "Square fitness : " << answers[i].fitness << "  Answer <" << i + 1 << "> : " << endl;
            for (int j = 0; j < SIZE; ++j)
            {
                for (int k = 0; k < SIZE; ++k)
                {
                    fw << answers[i].square[j][k] << " ";
                }
                fw << endl;
            }
            fw << endl
               << endl;
        }
        fw.close();
    }
    else
    {
        cout << "Problem With opening file!\n";
    }
}

int main()
{
    const int POPULATION_SIZE = 1000; // تعداد جمعیت
    const int ELITE_SIZE = 100;       // تعداد برتر حامعه برای نسل بعد
    const int OFFSPRING_SIZE = 900;   // تعداد فرزندان جدید در هر نسل
    const int MAX_GENERATIONS = 500; // ماکزیمم تعداد نسل ها

    srand((unsigned)(time(0)));

    vector<Individual> population = create_initial_population(POPULATION_SIZE);

    vector<Individual> answers;
    int generation = 0;
    Individual best_individual;

    while (generation < MAX_GENERATIONS)
    {
        vector<Individual> next_generation = generate_next_generation(population, ELITE_SIZE, OFFSPRING_SIZE);
        population = next_generation;
        best_individual = find_best_individual(population);
        ++generation;

        /*

        راه حلی برای اینکه جواب های مختلف چاپ شود
        به این صورت که کل جامعه را که به جواب رسیده را بیرون انداخته و دنبال جواب جدید میگردد

        */
        if (best_individual.fitness == 0 && !(is_in_answers(best_individual, answers)))
        {
            answers.push_back(best_individual);
            population.clear();
            population = create_initial_population(POPULATION_SIZE);
        }

        cout << "Generation : " << generation << "  Fitness: " << best_individual.fitness << endl;
        // cout << best_individual.fitness << endl;
    }

    // نمایش جواب نهایی
    print_square_answers(answers);

    cout << "last Fitness: " << best_individual.fitness << endl;

    system("pause");

    return 0;
}
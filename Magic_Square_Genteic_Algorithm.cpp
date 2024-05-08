#include <bits/stdc++.h>
using namespace std;
const int SIZE = 3; // سایز مربع جادویی

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
    int targetSum = SIZE * (SIZE * SIZE + 1) / 2;
    int fitness = 0;

    // حساب کردن محموع سطر
    for (int i = 0; i < SIZE; ++i)
    {
        int rowSum = 0;
        for (int j = 0; j < SIZE; ++j)
        {
            rowSum += square[i][j];
        }
        fitness += abs(rowSum - targetSum);
    }

    // حساب کردن محموع ستون
    for (int j = 0; j < SIZE; ++j)
    {
        int colSum = 0;
        for (int i = 0; i < SIZE; ++i)
        {
            colSum += square[i][j];
        }
        fitness += abs(colSum - targetSum);
    }

    // حساب کردن محموع قطر ها
    int diagonalSum1 = 0;
    int diagonalSum2 = 0;
    for (int i = 0; i < SIZE; ++i)
    {
        diagonalSum1 += square[i][i];
        diagonalSum2 += square[i][SIZE - 1 - i];
    }
    fitness += abs(diagonalSum1 - targetSum);
    fitness += abs(diagonalSum2 - targetSum);

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
    vector<Individual> sortedPopulation = population;
    sort(sortedPopulation.begin(), sortedPopulation.end(), [](const Individual &a, const Individual &b)
         { return a.fitness < b.fitness; });

    vector<Individual> selectedPopulation(sortedPopulation.begin(), sortedPopulation.begin() + elite_size);

    return selectedPopulation;
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
    Individual bestIndividual = population[0];

    for (const Individual &individual : population)
    {
        if (individual.fitness < bestIndividual.fitness)
        {
            bestIndividual = individual;
        }
    }

    return bestIndividual;
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

int main()
{
    const int POPULATION_SIZE = 1000; // تعداد جمعیت
    const int ELITE_SIZE = 20;        // تعداد برتر حامعه برای نسل بعد
    const int OFFSPRING_SIZE = 80;    // تعداد فرزندان جدید در هر نسل
    const int MAX_GENERATIONS = 1000; // ماکزیمم تعداد نسل ها

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
        به این صورت که جواب پیدا شده را پاک میکند و جای آن را با یک کروموزوم رندم پر میکند 
        تا آن جواب از لیست بیرون بیاید و دنبال جواب های جدید باشد
        */
        if (best_individual.fitness == 0 && !(is_in_answers(best_individual, answers)))
        {
            answers.push_back(best_individual);
            population.pop_back();
            Individual replace_individual;
            replace_individual.square = generate_random_square();
            replace_individual.fitness = calculate_fitness(replace_individual.square);
            population.push_back(replace_individual);
        }

        cout << "Generation : " << generation << "  Fitness: " << best_individual.fitness << endl;
    }

    cout << "Best individual:" << endl;

    // نمایش جواب نهایی 
    for (int i = 0; i < answers.size(); i++)
    {
        for (int j = 0; j < SIZE; ++j)
        {
            for (int k = 0; k < SIZE; ++k)
            {
                cout << best_individual.square[j][k] << " ";
            }
            cout << endl;
        }
        cout << endl
             << endl;
    }

    cout << "Fitness: " << best_individual.fitness << endl;

    return 0;
}
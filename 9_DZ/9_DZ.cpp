#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <numeric>

// Функция, вычисляющая значение подынтегральной функции
double integrand(double t) {
    return (cos(t) - 1) / t;
}

// Функция для подсчета интеграла на определенном интервале
double integrate(double a, double b, int N) {
    double h = (b - a) / N;
    double result = 0.0;

    for (int i = 0; i < N; ++i) {
        double x_i = a + i * h;
        result += integrand(x_i) * h;
    }

    return result;
}

// Функция для распараллеливания подсчета интеграла на нескольких потоках
double parallelIntegrate(double a, double b, int N, int numThreads) {
    std::vector<std::thread> threads;
    std::vector<double> results(numThreads, 0.0);
    std::mutex mutex;

    // Функция, которую выполняет каждый поток
    auto threadFunction = [&](int threadId) {
        int start = threadId * N / numThreads;
        int end = (threadId + 1) * N / numThreads;

        for (int i = start; i < end; ++i) {
            double x_i = a + i * (b - a) / N;
            results[threadId] += integrand(x_i) * (b - a) / N;
        }
    };

    // Создание потоков
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadFunction, i);
    }

    // Ожидание завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    // Суммирование результатов
    double result = std::accumulate(results.begin(), results.end(), 0.0);

    return result;
}

int main() {
    double x;
    int N;

    // Ввод параметров
    std::cout << "Enter x: ";
    std::cin >> x;
    std::cout << "Enter N: ";
    std::cin >> N;

    // Замер времени для последовательного подсчета интеграла
    auto startSequential = std::chrono::steady_clock::now();
    double resultSequential = integrate(0, x, N);
    auto endSequential = std::chrono::steady_clock::now();

    // Вывод результата и времени для последовательного подсчета
    std::cout << "Sequential result: " << resultSequential << std::endl;
    std::cout << "Sequential time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endSequential - startSequential).count() << " ms\n";

    // Подсчет интеграла на разном количестве потоков и замер времени
    for (int numThreads : {1, 2, 4, 8, 16, 32}) {
        auto startParallel = std::chrono::steady_clock::now();
        double resultParallel = parallelIntegrate(0, x, N, numThreads);
        auto endParallel = std::chrono::steady_clock::now();

        // Вывод результата и времени для параллельного подсчета
        std::cout << "Parallel result with " << numThreads << " threads: " << resultParallel << std::endl;
        std::cout << "Parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endParallel - startParallel).count() << " ms\n";
    }

    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.

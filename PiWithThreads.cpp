#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdint>

// Ilość wątków
int num_threads;

using FloatType = double; // Użycie podwójnej precyzji

FloatType calculate_integral(double start, double end, uint64_t num_steps) {
    FloatType step = (end - start) / num_steps;
    FloatType sum = 0.0;

    for (uint64_t i = 0; i < num_steps; ++i) {
        FloatType x = start + (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    return step * sum;
}

void thread_function(int thread_id, double a, double b, uint64_t num_steps, int thread_count, std::vector<FloatType>& results) {
    results[thread_id] = calculate_integral(a + thread_id * (b - a) / thread_count, a + (thread_id + 1) * (b - a) / thread_count, num_steps / thread_count);
}

int main() {
    const double a = 0.0;  // Początek przedziału całkowania
    const double b = 1.0;  // Koniec przedziału całkowania
    const uint64_t num_steps = 3000000000;  // Ilość kroków całkowania - używamy uint64_t, ponieważ long long int nie wystarcza

    int precision = 52;  // Ilość miejsc po przecinku

    /* Wczytywanie ilości miejsc po przecinku od użytkownika
    std::cout << "Podaj ilosc miejsc po przecinku: ";
    std::cin >> precision;
    */

    // Wczytywanie ilości wątków od użytkownika
    std::cout << "Podaj ilosc watkow: ";
    std::cin >> num_threads;

    std::vector<std::thread> threads;
    std::vector<FloatType> results(num_threads); // Przechowywanie wyników z poszczególnych wątków

    auto start_time = std::chrono::high_resolution_clock::now();

    // Tworzenie i uruchamianie wątków
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(thread_function, i, a, b, num_steps, num_threads, std::ref(results));
    }

    // Oczekiwanie na zakończenie wszystkich wątków
    for (auto& thread : threads) {
        thread.join();
    }

    FloatType result = 0.0;

    // Sumowanie wyników z wektora
    for (auto& result_thread : results) {
        result += result_thread;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    // Wyświetlanie wyniku z określoną ilością miejsc po przecinku
    std::cout << std::fixed << std::setprecision(precision);
    std::cout << "Wynik obliczen: " << result << std::endl;

    // Wyświetlanie czasu obliczeń w sekundach, milisekundach i mikrosekundach
    std::cout << "Czas obliczen: "
              << std::chrono::duration_cast<std::chrono::seconds>(duration).count() << " s, "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000 << " ms, "
              << duration.count() % 1000 << " us" << std::endl;

    return 0;
}

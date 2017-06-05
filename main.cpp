#include <iostream>
#include <cmath>

#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>

using namespace tbb;
using namespace std;

/* Обчислення підінтегральної функції. */
class integrand {
public:
    double operator()(double x) const {
        /* x^3 */
        return sin(x);
    }
};

/* Перевірка правила Рунге. */
bool check_Runge(double I2, double I, double epsilon) {
    return (fabs(I2 - I) / 3.) < epsilon;
}

/*
 * Об'єкт-функція, що виконує додавання елементів з проміжку r,
 * використовуючи значення start як початкове.
 */
template<typename Integrand>
class integrate_left_rectangle_func {
public:
    integrate_left_rectangle_func(Integrand f_, double epsilon_) :
            f(f_),
            epsilon(epsilon_) {}

    double operator()(const blocked_range<double> &r, double start) const {
        long long num_iterations = 1; /* Початкова кількість ітерацій */
        double last_res = 0.;   /* Результат інтeгрування на попередньому кроці */
        double res = -1.;       /* Поточний результат інтегрування */
        long double h = 0.;          /* Ширина прямокутників */
        cout << "Start=" << r.begin() << " End=" << r.end() << " epsilon=" << epsilon << endl;
        while (!check_Runge(res, last_res, epsilon)) {
            num_iterations *= 2;
            last_res = res;
            res = 0.;
            h = (r.end() - r.begin()) / num_iterations;
            for (long long i = 0; i < num_iterations; i++) {
                res += f(r.begin() + i * h);
//                if (isnan_double(res)) {
//                    cout << "res=" << f(r.begin() + i * h) << endl;
                    cout << "delta=" << fabs(res - last_res) / 3. << " num_iterations" << num_iterations << endl;
//                }
            }
//            cout << "Iterations=" << num_iterations << " Res=" << res << "; " << last_res << endl;
        }
        cout << "start=" << start << " res=" << res << " h=" << h << " num_iterations=" << num_iterations << endl;
        res *= h;
        cout << "start=" << start << " res=" << res << endl;
        return start + res;
    }

    Integrand f;
    double epsilon;
};

template<typename Integrand>
class integrate_left_rectangle_func1 {
public:
    integrate_left_rectangle_func1(Integrand f_, double epsilon_) :
            f(f_),
            epsilon(epsilon_) {}

    double run(double a, double b) const {
        long long num_iterations = 1; /* Початкова кількість ітерацій */
        double last_res = 0.;   /* Результат інтeгрування на попередньому кроці */
        double res = -1.;       /* Поточний результат інтегрування */
        long double h = 0.;          /* Ширина прямокутників */
        cout << "Start=" << a << " End=" << b << " epsilon=" << epsilon << endl;
        while (!check_Runge(res, last_res, epsilon)) {
            num_iterations *= 2;
            last_res = res;
            res = 0.;
            h = (b - a) / num_iterations;
            for (long long i = 0; i < num_iterations; i++) {
                res += f(a + i * h);
//                if (isnan_double(res)) {
//                    cout << "res=" << f(r.begin() + i * h) << endl;
                cout << "delta=" << fabs(res - last_res) / 3. << " num_iterations" << num_iterations << endl;
//                }
            }
//            cout << "Iterations=" << num_iterations << " Res=" << res << "; " << last_res << endl;
        }
        cout << "res=" << res << " h=" << h << " num_iterations=" << num_iterations << endl;
        res *= h;
        cout << "res=" << res << endl;
        return res;
    }

    Integrand f;
    double epsilon;
};


/*
 * Об'єкт-функція, що виконує додавання двох елементів
 */
class integrate_left_rectangle_reduction {
public:
    double operator()(double a, double b) const {
        return a + b;
    }
};

template<typename Integrand>
double parallel_integrate_left_rectangle(
        Integrand f, double start, double end, double epsilon) {
    return parallel_reduce(
            blocked_range<double>(start, end, 0),
            0.,
            integrate_left_rectangle_func<Integrand>(f, epsilon),
            integrate_left_rectangle_reduction());
}

int main() {
    integrate_left_rectangle_func1<integrand> iii = integrate_left_rectangle_func1<integrand>(integrand(), 1e-2);
    cout << iii.run(0, 5) << endl;

//    const int P_max = task_scheduler_init::default_num_threads();
//    cout << "Start. P_max=" << P_max << endl;
//    for (int P = 1; P <= P_max; P++) {
//        task_scheduler_init init(P);
//        tick_count t0 = tick_count::now();
//        cout << parallel_integrate_left_rectangle(integrand(), 0, 5, 1e-2)
//             << endl;
//        tick_count t1 = tick_count::now();
//        double t = (t1 - t0).seconds();
//        cout << "time = " << t << " with " << P << " threads" << endl;
//    }
}

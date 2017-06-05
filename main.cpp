#include <iostream>
#include <cmath>

#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>

using namespace std;

// Integrand
double f (double x)
{
    return x*x*x;
//    return 2*exp(2*x) / (sqrt(1+exp(2*x)));
}

// Refined calculation
double refined_calculation (long long n, double a, double b)
{
    double h = (b - a) / (double)n;

    double sum = (f(a) + f(b)) / 2;

    for (long long i = 1; i < n; i++)
        sum += f(a + i * h);

    return  h * sum;
}

/* Обчислення підінтегральної функції. */
class integrand
{
public:
    double operator()(double x) const
    {
        /* x^3 */
        return x*x*x;
    }
};

/* Перевірка правила Рунге. */
bool check_Runge(double I2, double I, double epsilon)
{
    return (fabs(I2 - I) / 3.) < epsilon;
}

/*
 * Об'єкт-функція, що виконує додавання елементів з проміжку r,
 * використовуючи значення start як початкове.
 */
template<typename Integrand>
class integrate_left_rectangle_func
{
public:
    integrate_left_rectangle_func(Integrand f_, double epsilon_):
            f(f_),
            epsilon(epsilon_)
    {}
    double operator()(const tbb::blocked_range<double> &r, double start) const
    {
        int num_iterations = 1; /* Початкова кількість ітерацій */
        double last_res = 0.;   /* Результат інтeгрування на попередньому кроці */
        double res = -1.;       /* Поточний результат інтегрування */
        double h = 0.;          /* Ширина прямокутників */
//        cout << "start=" << start << endl;
        while(!check_Runge(res, last_res, epsilon))
        {
            num_iterations *= 2;
            last_res = res;

//            cout << "res=" << res << " a=" << r.begin() << " b=" << r.end() << endl;
            res = refined_calculation(num_iterations, r.begin(), r.end());

//            h = (r.end() - r.begin()) / num_iterations;
//            for(int i = 0; i < num_iterations; i++)
//            {
//                res += f(r.begin() + i * h);
//            }
        }
//        res *= h;
        return start + res;
    }
    Integrand f;
    double epsilon;
};

/*
 * Об'єкт-функція, що виконує додавання двох елементів
 */
class integrate_left_rectangle_reduction
{
public:
    double operator()(double a, double b) const
    {
        return a + b;
    }
};

template<typename Integrand>
double parallel_integrate_left_rectangle(
        Integrand f, double start, double end, double epsilon)
{
    return tbb::parallel_reduce(
            tbb::blocked_range<double>(start, end, 0),
            0.,
            integrate_left_rectangle_func<Integrand>(f, epsilon),
            integrate_left_rectangle_reduction());
}

int main()
{
    const int P_max = tbb::task_scheduler_init::default_num_threads();
    for(int P = 1; P <= P_max; P++)
    {
        tbb::task_scheduler_init init(P);
        tbb::tick_count t0 = tbb::tick_count::now();
        std::cout << parallel_integrate_left_rectangle(integrand(), 0, 6, 1e-5)
                  << std::endl;
        tbb::tick_count t1 = tbb::tick_count::now();
        double t = (t1 - t0).seconds();
        std::cout << "time = " << t << " with " << P << " threads" << std::endl;
    }
}

//
// Distributed under the ITensor Library License, Version 1.2
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_TIMERS_H
#define __ITENSOR_TIMERS_H

#include <chrono>
#include "itensor/util/stdx.h"
#include "itensor/util/print.h"

#define COLLECT_TIMES

#ifdef COLLECT_TIMES
#define SCOPED_TIMER(N) auto scoped_timer_instance0_ = ScopedTimer(N);
#else
#define SCOPED_TIMER(N) 
#endif

#ifdef COLLECT_TIMES
#define START_TIMER(N) timers().start(N);
#else
#define START_TIMER(N) 
#endif

#ifdef COLLECT_TIMES
#define STOP_TIMER(N) timers().stop(N);
#else
#define STOP_TIMER(N) 
#endif

#ifdef COLLECT_TIMES
namespace itensor {

template<size_t NTimer>
struct Timers;

struct ScopedTimer;

using GlobalTimer = Timers<101ul>;

template<size_t NTimer>
struct Timers
    {
    using size_type = std::size_t;
    using duration_type = std::chrono::duration<double>;
    using clock_type = std::chrono::steady_clock;
    using now_type = decltype(clock_type::now());
    private:
    std::array<now_type,NTimer> start_;
    std::array<duration_type,NTimer> timer_;
    std::array<size_type,NTimer> count_;
    now_type gstart_;
    bool print_on_exit_ = false;
    public:

    Timers(bool print_on_exit = false)
      : print_on_exit_(print_on_exit)
        { 
        stdx::fill(timer_,duration_type::zero());
        stdx::fill(count_,0ul);
        gstart_ = now();
        }

    ~Timers()
        {
        if(print_on_exit_)
            println(*this);
        }

    void
    printOnExit(bool val) { print_on_exit_ = val; }

    size_type constexpr
    size() const { return NTimer; }

    void
    start(size_type n)
        {
        start_[n] = now(); 
        }

    void
    stop(size_type n)
        {
        auto end_n = now();
        timer_[n] += end_n-start_[n];
        count_[n] += 1ul;
        }

    size_t
    count(size_type n) const { return count_[n]; }

    double
    time(size_type n) const { return timer_[n].count(); }

    double
    avgTime(size_type n) const 
        { 
        if(count_[n] == 0) return 0;
        return timer_[n].count()/count_[n]; 
        }

    double
    total() const 
        { 
        auto end = now();
        duration_type d = (end-gstart_);
        return d.count();
        }

    private:

    now_type
    now() const { return clock_type::now(); }

    };

inline GlobalTimer & 
timers()
    {
    static GlobalTimer timers_(true);
    return timers_;
    }

struct ScopedTimer
    {
    size_t N = 0ul;
    ScopedTimer(size_t N_) 
      : N(N_)
        { 
        timers().start(N);
        }
    ~ScopedTimer()
        {
        timers().stop(N);
        }
    };

template<size_t NTimer>
std::ostream&
operator<<(std::ostream& s, Timers<NTimer> const& T)
    {
    auto tot = T.total();
    s << "-----------------------------------------------------\n";
    s << format("Timers:                  Total Time = %.4f  %% Total",tot);
    for(decltype(T.size()) n = 0; n < T.size(); ++n)
        {
        if(T.count(n) == 0) continue;
        auto avg = T.avgTime(n);
        auto time = T.time(n);
        auto pct = 100*(time/tot);
        s << format("\nSection %2d, Average = %.4f, Total = %.4f [%5.1f%%]",n,avg,time,pct);
        }
    s << "\n-----------------------------------------------------";
    return s;
    }

} //namespace itensor
#endif //COLLECT_TIMES

#endif

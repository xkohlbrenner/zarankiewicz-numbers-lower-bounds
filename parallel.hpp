#pragma once

#include <algorithm>
#include <atomic>
#include <future>
#include <optional>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <functional>
#include <random>
#include <thread>

const int THREADS = -1;

//created with AI support
namespace parallel {

template <class F>
auto bestOfParallel(int iterations, F solve)
{
    using Ret = std::decay_t<std::invoke_result_t<F&>>;

    if (iterations <= 0) throw std::invalid_argument("For parallelization: iterations must be > 0");

    auto better = [](const Ret& a, const Ret& b) { return std::get<1>(a) > std::get<1>(b); };

    const unsigned nThreads = std::clamp(THREADS == -1 ? std::thread::hardware_concurrency() : static_cast<unsigned>(THREADS), 1u, static_cast<unsigned>(iterations));

    std::atomic<int> nextRun{0};
    std::vector<std::future<std::optional<Ret>>> futures;
    futures.reserve(nThreads);

    for (unsigned t = 0; t < nThreads; ++t) {
        futures.push_back(std::async(std::launch::async, [&] {
            std::optional<Ret> best;
            while (nextRun.fetch_add(1, std::memory_order_relaxed) < iterations) {
                Ret ret = solve();
                if (!best || better(ret, *best))
                    best = std::move(ret);
            }
            return best;
        }));
    }

    std::optional<Ret> best;
    for (auto& f : futures) {
        std::optional<Ret> ret = f.get();
        if (ret && (!best || better(*ret, *best)))
            best = std::move(ret);
    }
    return std::move(*best);
}

}

inline std::mt19937& rng() {
    thread_local std::mt19937 gen{
        std::random_device{}() ^
        static_cast<unsigned>(std::hash<std::thread::id>{}(std::this_thread::get_id()))};
    return gen;
}
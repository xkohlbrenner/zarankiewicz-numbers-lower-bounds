#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <deque>
#include <stack>
#include <thread>
#include <deque>
#include <cmath>
#include <climits>
#include <utility>
#include <tuple>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <memory>
#include <cstddef>
#include <chrono>


#include "structures.hpp"
#include "io.hpp"
#include "parallel.hpp"




using namespace std;

using ll  = long long;
using ull = unsigned long long;
using pii = pair<int, int>;
using vi  = vector<int>;
using vll = vector<ll>;


template <typename T>
void printVector(const std::vector<T>& v) {
    std::cout << '[';
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << v[i];
    }
    std::cout << "]\n";
}

void printPosition(const position& p) {
    std::cout << "(" << p.row << ", " << p.column << ")\n";
}

//AI generated function to find the indices of the maximum values in a vector
std::vector<int> maxIndizes(const std::vector<int>& v) {
    std::vector<int> indizes;
    if (v.empty()) return indizes;

    int maximum = v[0];
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (v[i] > maximum) {
            maximum = v[i];
            indizes.clear();         
            indizes.push_back(i);
        } else if (v[i] == maximum) {
            indizes.push_back(i);     
        }
    }
    return indizes;
}

template <typename T>
std::vector<T> selectByIndices(const std::vector<T>& values, const std::vector<int>& indices) {
    std::vector<T> result;
    result.reserve(indices.size());
    for (int i : indices)
        result.push_back(values[i]);
    return result;
}

position bestCandidate(const std::vector<position>& candidates, const Matrix<Edge>& edges, int m, int n, bool random = false) {
    std::vector<position> bestCandidates;
    int bestScore = -1;

    for (const position& c : candidates) {
        int score = edges(c.row, c.column).KSTs.size();
        if (score > bestScore) {
            bestScore = score;
            bestCandidates.clear();
            bestCandidates.push_back(c);
        } else if (score == bestScore) {
            bestCandidates.push_back(c);
        }
    }
    if (random) {
        std::uniform_int_distribution<std::size_t> dist(0, bestCandidates.size() - 1);
        return bestCandidates[dist(rng())];
    }
    return bestCandidates[0];
}

std::vector<position> findCandidates(Matrix<Edge>& edges, int m, int n) {
    std::vector<position> result;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (edges(i, j).active && edges(i, j).KSTs.size() > 0) {
                result.push_back({i, j});
            }
        }
    }
    return result;
}


std::vector<int> onePartner(KSTSet& kstSet, std::vector<position>& candidates, int m, int n) {
    std::vector<int> result;

    for (const position& c : candidates) {
        int count = 0;
        for (const KST* kst : kstSet.adj(c.row, c.column).KSTs) {
            count += kst->soloEdges;
        };
        result.push_back(count);
    }
    return result;
}



std::uint64_t binomial(std::uint64_t n, std::uint64_t k) {
    if (k > n) return 0;
    if (k > n - k) k = n - k;          
    std::uint64_t result = 1;
    for (std::uint64_t i = 1; i <= k; ++i) {
        result = result * (n - k + i) / i;
    }
    return result;
}




std::tuple<std::string, int> solveGreedy(int m, int n, int s, int t, bool onePartners, bool random) {
    KSTSet kstSet = generateKSTs(m, n, s, t);

    std::vector<position> candidates = findCandidates(kstSet.adj, m, n);
    position chosenEdge;
    while (candidates.size() > 0) {    
        if (onePartners) {
            //Find the candidates the most partners that are in only one KST
            std::vector<int> priorityCandidates_Index = maxIndizes(onePartner(kstSet, candidates, m, n));
            std::vector<position> priorityCandidates = selectByIndices(candidates, priorityCandidates_Index);
            chosenEdge = bestCandidate(priorityCandidates, kstSet.adj, m, n);
        } else {
            chosenEdge = bestCandidate(candidates, kstSet.adj, m, n, random);
        }


        kstSet.adj(chosenEdge.row, chosenEdge.column).active = false;
        for (KST* kst : kstSet.adj(chosenEdge.row, chosenEdge.column).KSTs) {
            kst->alive = false;
            for (Edge* e : kst->edges) {
                e->removeKST(kst);
            }
        }
        if (onePartners) {
            for (int i = 0; i < m; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (kstSet.adj(i, j).KSTs.size() == 1) {
                        kstSet.adj(i, j).KSTs[0]->soloEdges++;
                    }
                }
            }
        }

        candidates = findCandidates(kstSet.adj, m, n);
    }

    std::string outputMatrix = to_graph6(kstSet.adj, [](const Edge& e) { return e.active; });

    return std::make_tuple(outputMatrix, std::count_if(kstSet.adj.begin(), kstSet.adj.end(), [](const Edge& e) {return e.active;}));
    

}

int main(int argc, char* argv[]) {
    const auto start = std::chrono::steady_clock::now();
    
    const int totalIterations = 100;
    const unsigned nThreads = std::thread::hardware_concurrency();
    



    Options opt;

    try {
        opt = parseArgs(argc, argv);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n";
        printUsage(argv[0]);
        return 1;
    }

    const auto& [m, n, s, t] = opt.values;

    std::string outputMatrix;
    int edges = 0;

    if (opt.greedy) {
        std::tie(outputMatrix, edges) = solveGreedy(m, n, s, t, false, false);
    } else if (opt.onePartners) {
        std::tie(outputMatrix, edges) = solveGreedy(m, n, s, t, true, false);
    } else if (opt.random) {
        std::tie(outputMatrix, edges) = parallel::bestOfParallel(totalIterations, [&]() { return solveGreedy(m, n, s, t, false, true);});
        /*
        int currentEdges;
        std::string currentMatrix;
        for (int i = 0; i < 100; ++i) {
            std::tie(currentMatrix, currentEdges) = solveGreedy(m, n, s, t, false, true);
            if (currentEdges > edges) {
                outputMatrix = currentMatrix;
                edges = currentEdges;
            }
        }
        */
    }

    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed = end - start;

    std::cout << "Graph6 representation: " << outputMatrix << std::endl;
    std::cout << "Number of edges: " << edges << std::endl;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
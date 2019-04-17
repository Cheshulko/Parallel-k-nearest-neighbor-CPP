#include <chrono>
#include <cmath>
#include <fstream>
#include <future>
#include <iostream>
#include <set>
#include <string>
#include <thread>
#include <vector>

class Point {
   public:
    Point(int id, double x, double y, double z) : id(id), x(x), y(y), z(z){};
    Point() { Point(-1, 0, 0, 0); };

    double euclideanDistance(const Point& point) const {
        double x = this->x - point.x;
        double y = this->y - point.y;
        double z = this->z - point.z;
        return sqrt(x * x + y * y + z * z);
    }

    friend std::ostream& operator<<(std::ostream& out, const Point& point);
    friend std::istream& operator>>(std::istream& in, Point& point);

    int id;
    double x, y, z;
};

std::istream& operator>>(std::istream& is, Point& point) {
    is >> point.x >> point.y >> point.z;
    return is;
}

std::ostream& operator<<(std::ostream& out, const Point& point) {
    out << point.x << " " << point.y << " " << point.z;
    return out;
}

using ResultGrid = std::vector<std::vector<int>>;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout
            << "Wrong input. Use ./main <points_file.txt> <result_file.txt>"
            << std::endl;
        return 0;
    }
    std::ifstream inFile;
    inFile.open(argv[1]);
    std::ofstream outFile;
    outFile.open(argv[2]);

    if (!inFile.is_open() || !outFile.is_open()) {
        std::cout << "Wrong files." << std::endl;
        return 0;
    }

    int neighbors, threads, points;

    std::cout << "Enter threads: ";
    std::cin >> threads;
    std::cout << "Enter neighbors: ";
    std::cin >> neighbors;
    inFile >> points;

    std::cout << "Start processing..." << std::endl;
    auto startTime = std::chrono::system_clock::now();

    int perThread = points / threads;
    int lastThread = perThread + points % threads;

    std::vector<int> threadRows(threads, perThread);
    threadRows.back() = lastThread;

    std::shared_ptr<std::vector<Point>> ptrPoints(
        new std::vector<Point>(points));
    std::shared_ptr<ResultGrid> ptrResultGrid(new ResultGrid(points));

    int pointIndex = 1;
    for (auto& point : *ptrPoints) {
        inFile >> point;
        point.id = pointIndex++;
    }

    auto calcKNearestNeighbors = [ptrPoints, ptrResultGrid, &neighbors](
                                     int indexFrom, int indexTo) mutable {
        std::set<std::pair<double, int>> minSet;
        double distance = 0;
        int pointIndex = -1;
        int curNeighbor = 0;

        for (int point = indexFrom; point < indexTo; ++point) {
            minSet.clear();

            pointIndex = -1;
            for (auto& pointTo : *ptrPoints) {
                ++pointIndex;
                if (point == pointIndex) continue;
                distance = (*ptrPoints)[point].euclideanDistance(pointTo);
                minSet.insert({distance, pointTo.id});
            }

            curNeighbor = 0;
            for (auto setIter = minSet.begin();
                 setIter != minSet.end() && curNeighbor < neighbors;
                 ++setIter, ++curNeighbor) {
                (*ptrResultGrid)[point].push_back(setIter->second);
            }
        }
    };

    std::vector<std::future<void>> futuresResult;

    int startIndex = 0;
    for (auto& threadRow : threadRows) {
        futuresResult.push_back(std::async(std::launch::async,
                                           calcKNearestNeighbors, startIndex,
                                           startIndex + threadRow));
        startIndex += threadRow;
    }

    for (auto& future : futuresResult) {
        future.get();
    }

    for (auto& neighborsList : *ptrResultGrid) {
        for (auto& neighbors : neighborsList) {
            outFile << neighbors << " ";
        }
        outFile << std::endl;
    }

    inFile.close();
    outFile.close();

    auto delta = std::chrono::system_clock::now() - startTime;
    std::cout << "Done." << std::endl;
    std::cout
        << "Time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()
        << " milliseconds" << std::endl;

    return 0;
}

// g++ -std=c++17 main.cpp -o main
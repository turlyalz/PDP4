#ifndef BASE_H
#define BASE_H

#include <vector>
#include <iostream>

struct Solution;
struct Problem;

std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<bool>>& graph);

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& nodes);

std::ostream& operator<<(std::ostream& os, const Problem* problem);

std::ostream& operator<<(std::ostream& os, const Solution* solution);

#endif // BASE_H


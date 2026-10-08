#include "io.h"

std::ostream& operator<<(std::ostream& os, const position& p) {
    return os << '(' << p.row << ", " << p.column << ')';
}
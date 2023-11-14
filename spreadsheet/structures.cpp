#include "common.h"

#include <cctype>
#include <sstream>
#include <algorithm>
#include <iostream>

const int LETTERS = 26;

const int MAX_POSITION_LENGTH = 17;

const int MAX_POS_LETTER_COUNT = 3;

const Position Position::NONE = {-1, -1};

bool Position::operator==(const Position rhs) const {
    return (row == rhs.row) && (col == rhs.col);
}

bool Position::operator<(const Position rhs) const {
    return (row < rhs.row) || ((row == rhs.row) && (col < rhs.col));
}

bool Position::IsValid() const {
    return (row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLS);
}

std::string Position::ToString() const {
    if (!IsValid()) {
        return "";
    }
    std::string result;
    int colNum = col + 1;
    while (colNum > 0) {
        int remainder = (colNum - 1) % LETTERS;
        char letter = 'A' + remainder;
        result = letter + result;
        colNum = (colNum - 1) / LETTERS;
    }
    result += std::to_string(row + 1);
    return result;
}

Position Position::FromString(std::string_view str) {
    auto pos = std::find_if(str.begin(), str.end(), [](auto ch){ return isdigit(ch);});
    auto n = std::distance(str.begin(), pos);
    std::string letters = std::string(str.substr(0, n));
    std::reverse(letters.begin(), letters.end());
    std::string digits = std::string(str.substr(n));
    if(std::find_if(digits.begin(), digits.end(), [](auto ch){ return !isdigit(ch);}) != digits.end()
       || digits.size() > 5 || letters.empty() || pos == str.end()) {
        return Position::NONE;
    }
    int col = -1;
    int i = 1;
    for(const char ch : letters) {
        if(!std::isupper(ch)) {
            return Position::NONE;
        }
        col += (ch - 'A' + 1) * i;
        i *= LETTERS;
    }
    int row = std::stoi(digits) - 1;
    Position res{ row, col };
    return (res.IsValid()) ? res : Position::NONE;
}

bool Size::operator==(Size rhs) const {
    return rows == rhs.rows && cols == rhs.cols;
}

FormulaError::FormulaError(FormulaError::Category category) : category_(category) {}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
    std::string_view res;
    if(category_ == Category::Div0) {
        res = "DIV/0";
    } else if (category_ == Category::Ref) {
        res = "REF";
    } else if (category_ == Category::Value) {
        res = "VALUE";
    }
    return res;
}

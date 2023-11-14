#include "sheet.h"

using namespace std::literals;

Sheet::~Sheet() = default;

void Sheet::MaybeIncreaseSizeToIncludePosition(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid Position");
    }
    int row = pos.row + 1;
    int col = pos.col + 1;
    if (row> static_cast<int>(cells_.size())) {
        cells_.resize(row);
    }
    if (col > static_cast<int>(cells_[pos.row].size())) {
        cells_[pos.row].resize(col);
    }
    if (row > sheet_size_.rows) {
        sheet_size_.rows = row;
    }
    if (col > sheet_size_.cols) {
        sheet_size_.cols = col;
    }
}

void Sheet::SetCell(Position pos, std::string text) {
    MaybeIncreaseSizeToIncludePosition(pos);
    std::unordered_map<Cell*, State> cells;
    if(GetCell(pos) == nullptr) {
        auto& new_cell = cells_[pos.row][pos.col];
        new_cell = std::make_unique<Cell>(*this, pos);
        new_cell->Set(text);
        if(CheckCyclicDependencies(GetConcreteCell(pos), cells)) {
            new_cell.reset();
            throw CircularDependencyException("Circular Dependency");
        }
    } else if(GetCell(pos)->GetText() != text) {
        auto& cell = cells_[pos.row][pos.col];
        if (cell->IsReversedReferenced()) {
            CacheInvalidation(cell.get());
        }
        std::string prev = cell->GetText();
        cell->Clear();
        cell->Set(text);
        if(CheckCyclicDependencies(GetConcreteCell(pos), cells)) {
            cell->Set(prev);
            throw CircularDependencyException("Circular Dependency");
        }
    } else {
        return;
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return GetConcreteCell(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    return GetConcreteCell(pos);
}

const Cell* Sheet::GetConcreteCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid Position");
    }
    if (pos.row + 1 > static_cast<int>(cells_.size()) || pos.col + 1 > static_cast<int>(cells_[pos.row].size())) {
        return nullptr;
    }
    const auto& cell = cells_[pos.row][pos.col];
    return cell ? cell.get() : nullptr;
}

Cell* Sheet::GetConcreteCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid Position");
    }
    if (pos.row + 1 > static_cast<int>(cells_.size()) || pos.col + 1 > static_cast<int>(cells_[pos.row].size())) {
        return nullptr;
    }
    auto& cell = cells_[pos.row][pos.col];
    return cell ? cell.get() : nullptr;
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid Position");
    }
    if (GetCell(pos) == nullptr) {
        return;
    }
    cells_[pos.row][pos.col].reset();
    if (pos.row == sheet_size_.rows - 1 || pos.col == sheet_size_.cols - 1) {
        sheet_size_.rows = 0;
        sheet_size_.cols = 0;
        for (size_t row = 0; row < cells_.size(); ++row) {
            for (size_t col = 0; col < cells_[row].size(); ++col) {
                if (cells_[row][col]) {
                    sheet_size_.rows = std::max(sheet_size_.rows, static_cast<int>(row) + 1);
                    sheet_size_.cols = std::max(sheet_size_.cols, static_cast<int>(col) + 1);
                }
            }
        }
    }
}

Size Sheet::GetPrintableSize() const {
    return sheet_size_;
}

void Sheet::PrintCells(std::ostream& output, const std::function<void(const CellInterface&)>& printCell) const {
    for (int row = 0; row < sheet_size_.rows; ++row) {
        for (int col = 0; col < sheet_size_.cols; ++col) {
            auto cell = GetCell({row, col});
            if (cell != nullptr) {
                printCell(*cell);
            }
            if (col != sheet_size_.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}

void Sheet::PrintValues(std::ostream& output) const {
    auto func = [&output](const CellInterface& cell)
            {std::visit([&output](const auto& x) { output << x; }, cell.GetValue()); };
    PrintCells(output, func);
}

void Sheet::PrintTexts(std::ostream& output) const {
    auto func = [&output](const CellInterface& cell){ output << cell.GetText(); };
    PrintCells(output, func);
}


std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

void Sheet::CacheInvalidation(Cell* cell) {
    if(!cell->HasCachedValue()) {
        return;
    } else {
        cell->ClearCache();
        if(!cell->IsReversedReferenced()) {
            return;
        } else {
            for(const auto ref : cell->GetReversedReferencedCells()) {
                CacheInvalidation(GetConcreteCell(ref));
            }
        }
    }
}

bool Sheet::CheckCyclicDependencies(Cell* cell, std::unordered_map<Cell*, State>& cells) {
    cells.insert({cell, State::grey});
    for(auto ref : cell->GetReferencedCells()) {
        auto ref_cell = GetConcreteCell(ref);
        if(cells.find(ref_cell) != cells.end()) {
            auto ref_state = cells.find(ref_cell)->second;
            if(ref_state == State::grey) {
                return true;
            }
            if(ref_state == State::black) {
                return false;
            }
        }
        if(CheckCyclicDependencies(ref_cell, cells)) {
            return true;
        }
    }
    cells.find(cell)->second = State::black;
    return false;
}
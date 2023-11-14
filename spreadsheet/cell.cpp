#include "cell.h"

Cell::Cell(SheetInterface& sheet, Position position) : sheet_(sheet), position_(position) {
    impl_ = std::make_unique<EmptyImpl>(EmptyImpl());
}

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    if (text[0] == FORMULA_SIGN && text.size() > 1) {
        impl_ = std::make_unique<FormulaImpl>(FormulaImpl(text, sheet_));
        referenced_cells_ = impl_->GetReferencedCells();
        for(const auto ref_cell : referenced_cells_) {
            if(sheet_.GetCell(ref_cell) == nullptr) {
                sheet_.SetCell(ref_cell, "");
            }
            dynamic_cast<Cell *>(sheet_.GetCell(ref_cell))->reversed_referenced_cells_.push_back(position_);
        }
    } else {
        impl_ = std::make_unique<TextImpl>(TextImpl(text));
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>(EmptyImpl());
    referenced_cells_.clear();
    reversed_referenced_cells_.clear();
}

Cell::Value Cell::GetValue() const {
    if(cache_.has_value()) {
        return cache_.value();
    } else {
        return impl_->GetValue();
    }
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

bool Cell::HasCachedValue() const {
    return cache_.has_value();
}

void Cell::ClearCache() {
    cache_ = std::nullopt;
}

const std::vector<Position>& Cell::GetReferencedCells() const {
    return referenced_cells_;
}

bool Cell::IsReferenced() const {
    return !referenced_cells_.empty();
}

const std::vector<Position>& Cell::GetReversedReferencedCells() const {
    return reversed_referenced_cells_;
}

bool Cell::IsReversedReferenced() const {
    return !reversed_referenced_cells_.empty();
}

#pragma once

#include "common.h"
#include "formula.h"

#include <optional>
#include <vector>

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);

    ~Cell();

    void Set(std::string text);

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;

    std::vector<Cell*> GetReferencedCells() const override;

    bool IsReferenced() const;

    const std::vector<Cell*>& GetReversedReferencedCells() const;

    bool IsReversedReferenced() const;

private:
    class Impl;

    class EmptyImpl;

    class TextImpl;

    class FormulaImpl;

    std::unique_ptr<Impl> impl_;

    mutable std::optional<double> cache_;

    std::vector<Cell*> referenced_cells;
    
    std::vector<Cell*> reversed_referenced_cells;
};
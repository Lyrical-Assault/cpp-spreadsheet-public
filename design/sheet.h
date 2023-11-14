#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <vector>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;

    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;

    void PrintTexts(std::ostream& output) const override;

    bool ChechCyclicDependencies(Cell* cell, std::vector<Cell*>& cells) const;

    void CacheInvalidation(Cell* cell) const;

private:
    Size sheet_size_;
    
    std::vector<std::vector<std::unique_ptr<Cell>>> cells_;
};
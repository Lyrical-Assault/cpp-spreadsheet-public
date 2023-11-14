#pragma once

#include "common.h"
#include "formula.h"

#include <optional>
#include <vector>
#include <cassert>
#include <iostream>
#include <string>

class Sheet;

class Cell : public CellInterface {
public:
    explicit Cell(SheetInterface& sheet, Position position);

    ~Cell();

    void Set(std::string text) override;

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;

    bool HasCachedValue() const;

    void ClearCache();

    const std::vector<Position>& GetReferencedCells() const override;

    bool IsReferenced() const;

    const std::vector<Position>& GetReversedReferencedCells() const;

    bool IsReversedReferenced() const;

private:
    class Impl {
    public:
        Impl() = default;
        virtual ~Impl() = default;

        virtual Cell::Value GetValue() const = 0;

        virtual std::string GetText() const = 0;

        virtual std::vector<Position> GetReferencedCells() const = 0;
    };

    class EmptyImpl : public Impl {
    public:
        EmptyImpl() = default;

        Cell::Value GetValue() const override {
            return std::string("");
        }

        std::string GetText() const override {
            return std::string("");
        }

        std::vector<Position> GetReferencedCells() const override {
            return {};
        }

    };

    class TextImpl : public Impl {
    public:
        TextImpl(std::string text) : text_(text) {}

        Cell::Value GetValue() const override {
            if(text_[0] == ESCAPE_SIGN) {
                return text_.substr(1);
            } else {
                return text_;
            }
        }

        std::string GetText() const override {
            return text_;
        }

        std::vector<Position> GetReferencedCells() const override {
            return {};
        }

    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::string text, const SheetInterface& sheet) :  sheet_(sheet), formula_(ParseFormula(text.substr(1))) {}

        Cell::Value GetValue() const override {
            auto value = formula_->Evaluate(sheet_);
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            } else {
                return std::get<FormulaError>(value);
            }
        }

        std::string GetText() const override {
            return "=" + formula_->GetExpression();
        }

        std::vector<Position> GetReferencedCells() const override {
            return formula_->GetReferencedCells();
        }

    private:
        const SheetInterface& sheet_;

        std::unique_ptr<FormulaInterface> formula_;
    };

    std::unique_ptr<Impl> impl_;

    SheetInterface& sheet_;

    Position position_;

    std::optional<double> cache_;

    std::vector<Position> referenced_cells_;
    
    std::vector<Position> reversed_referenced_cells_;
};
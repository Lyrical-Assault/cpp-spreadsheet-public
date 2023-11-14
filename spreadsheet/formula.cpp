#include "formula.h"

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {
    class Formula : public FormulaInterface {
    public:
        explicit Formula(std::string expression)
            try : ast_(ParseFormulaAST(expression)) {
            } catch (...) {
                throw FormulaException("Incorrect formula");
            }

        Value Evaluate(const SheetInterface& sheet) const override {
            auto func = [&sheet](Position pos){
                auto cell = sheet.GetCell(pos);
                double res = 0.0;
                if(cell == nullptr) {
                    return res;
                }
                auto value = cell->GetValue();
                if(std::holds_alternative<double>(value)) {
                    res = std::get<double>(value);
                } else if(std::holds_alternative<std::string>(value)) {
                    std::string str_value = std::get<std::string>(value);
                    if(str_value.empty()) {
                        return res;
                    }
                    if(!std::isdigit(str_value.front())) {
                        throw FormulaError(FormulaError::Category::Value);
                    }
                    std::size_t sz;
                    res = std::stod(str_value, &sz);
                    if(sz < str_value.size()) {
                        throw FormulaError(FormulaError::Category::Value);
                    }
                } else if(std::holds_alternative<FormulaError>(value)) {
                    throw FormulaError(FormulaError::Category::Value);
                }
                return res; };
            try {
                return ast_.Execute(func);
            } catch (const FormulaError& err) {
                return err;
            }
        }

        std::string GetExpression() const override {
            std::ostringstream str;
            ast_.PrintFormula(str);
            return str.str();
        }

        std::vector<Position> GetReferencedCells() const override {
            std::vector<Position> referenced_cells;
            for(const auto cell : ast_.GetCells()) {
                referenced_cells.push_back(cell);
            }
            return {referenced_cells.begin(), std::unique(referenced_cells.begin(), referenced_cells.end())};
        }

    private:
        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}

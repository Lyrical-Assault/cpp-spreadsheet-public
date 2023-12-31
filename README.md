# cpp-spreadsheet
## Дипломный проект: Электронная таблица

Электронная таблица поддерживает два вида ячеек:
- Формульная — ячейка трактуется как формульная, если её текст начинается со знака "=". Пробелы перед знаком "=" не игнорируются;
- Текстовая — все ячейки, кроме формульных.

К ячейке можно обращаться по индексу методом GetCell, устанавливать значение методом SetCell и очищать ячейку методом ClearCell. Пользователь может получить размер минимальной печатной области таблицы с помошью метода GetPrintableSize, а также напечатать таблицу в поток вывода с помощью методов PrintText, который печатает текстовое представление ячеек, и PrintValues, который выводит значения ячеек.

Электронная таблица эффективна по памяти в случае, если таблица разрежена (большая минимальная печатная область и много пустых ячеек), не вызывает утечек памяти при удалении ячеек или таблицы целиком и предоставляет доступ к своим ячейкам по индексу Position за O(1). Таблица гарантирует согласованное состояние, не допускает появления циклических зависимостей.

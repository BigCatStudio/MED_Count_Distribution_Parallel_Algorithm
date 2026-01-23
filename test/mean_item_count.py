#!/usr/bin/env python3
from pathlib import Path
import sys

def avg_elements_per_line(path: str) -> None:
    p = Path(path)

    total_elems = 0
    counted_lines = 0
    empty_lines = 0
    min_len = None
    max_len = None

    with p.open("r", encoding="utf-8") as f:
        for line in f:
            stripped = line.strip()  # usuwa spacje z początku i końca (w tym końcowe spacje)
            if not stripped:
                empty_lines += 1
                continue

            n = len(stripped.split(','))  # split() bez argumentu: po dowolnych białych znakach
            total_elems += n
            counted_lines += 1

            min_len = n if min_len is None else min(min_len, n)
            max_len = n if max_len is None else max(max_len, n)

    if counted_lines == 0:
        print("Brak niepustych wierszy do policzenia.")
        return

    avg = total_elems / counted_lines
    print(f"Średnio elementów na niepusty wiersz: {avg:.3f}")
    print(f"Niepuste wiersze: {counted_lines}")
    print(f"Puste wiersze: {empty_lines}")
    print(f"Min elementów w wierszu: {min_len}")
    print(f"Max elementów w wierszu: {max_len}")

def main():
    if len(sys.argv) != 2:
        print("Użycie: python avg_elems.py PLIK.txt", file=sys.stderr)
        sys.exit(1)

    avg_elements_per_line(sys.argv[1])

if __name__ == "__main__":
    main()

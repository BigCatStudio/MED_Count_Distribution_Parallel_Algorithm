#!/usr/bin/env python3
from pathlib import Path
import sys

def convert_spaces_to_commas(input_path: str, output_path: str) -> None:
    inp = Path(input_path)
    outp = Path(output_path)

    with inp.open("r", encoding="utf-8") as f_in, outp.open("w", encoding="utf-8", newline="\n") as f_out:
        for line in f_in:
            # usuń białe znaki na końcu linii (w tym spacje)
            stripped = line.rstrip()

            # zachowaj puste linie (jeśli są)
            if not stripped:
                f_out.write("\n")
                continue

            # split() bez argumentu rozcina po dowolnych białych znakach i ignoruje ich wielokrotność
            parts = stripped.split()
            f_out.write(",".join(parts) + "\n")

def main():
    if len(sys.argv) != 3:
        print("Użycie: python convert.py WEJSCIE.txt WYJSCIE.txt", file=sys.stderr)
        sys.exit(1)

    convert_spaces_to_commas(sys.argv[1], sys.argv[2])

if __name__ == "__main__":
    main()

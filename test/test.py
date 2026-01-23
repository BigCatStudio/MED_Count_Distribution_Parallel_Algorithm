from apyori import apriori

file_name = "./data/"
raw = None
# with open("./data/transactional_T10I4D100K.csv", "r") as file:
with open("./data/retails_small.txt", "r") as file:
    raw = file.read()

# parsowanie
transactions = [
    [item.strip() for item in line.split(",") if item.strip()]
    for line in raw.splitlines()
    if line.strip()
]

# apriori
results = list(apriori(
    transactions,
    min_support=0.05,
    min_confidence=0.75,
    min_lift=1.0,
    min_length=2
))

# uzyskanie reguły asocjacyjne
counter = 0
for r in results:
    support = r.support
    for stat in r.ordered_statistics:
        lhs = tuple(stat.items_base)
        rhs = tuple(stat.items_add)
        if lhs and rhs:
            counter += 1
            print(f"{lhs} -> {rhs} | "
                  f"support={support:.3f} | "
                  f"confidence={stat.confidence:.3f} | "
                  f"lift={stat.lift:.3f}")
print("Counter:", counter)

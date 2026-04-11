# This is just a test file with ONLY find implemented
# Comments are in Russian for the jokes

from numpy import int8


class BackingArray:
    def __init__(self, capacity) -> None:
        self.control = [0 for _ in range(capacity)]
        self.slots = [int8(-128) for _ in range(capacity)]


class SwissTable:
    def __init__(self) -> None:
        self.groups = 2
        self.backing_array = BackingArray(capacity=self.capacity())

    def capacity(self):
        return self.groups * 16

    def find(self, value):
        vhash = hash(value)
        h1 = vhash
        h2 = vhash & ((1 << 8) - 1)
        # print(bin(vhash), bin(h1), bin(h2))
        group_ind = h1 & self.groups
        matches = self.getMatches(group_ind, h2)
        for i, el in enumerate(matches):
            if self.backing_array.slots[group_ind * 16 + i] == value:
                return (group_ind, i)
        return -1

    def getMatches(self, group_ind, h2):
        matches = [
            1 if (h2 == hash(el) & ((1 << 8) - 1)) else 0
            for el in self.backing_array.control[group_ind * 16 : group_ind * 16 + 16]
        ]
        return matches

    def insert(self, value):
        is_already_in = self.find(value)
        if is_already_in != -1:
            return is_already_in
        vhash = hash(value)
        h1 = vhash
        h2 = vhash & ((1 << 8) - 1)
        group_ind = h1 & self.groups
        # индиффирентно на переполнение
        #
        # НА ЭТОМ МОИ ПОПЫТКИ СИМУЛЯЦИИ НА PYTHON ПОДОШЛИ К КОНЦУ
        #


table = SwissTable()
print(table.find("osaskd"))

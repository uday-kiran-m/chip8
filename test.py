# filename = "test.ch8"


# def read_opcodes(filename):
#     opcodes = []
#     with open(filename, "rb") as f:
#         # print(f.read())
#         data = list(f.read())
#         for i in range(0, len(data), 2):
#             opcodes.append(hex(data[i]*256 + data[i+1])[2:])

#     return opcodes


# print(read_opcodes(filename))

print(0x15B%0x10)
def xlat(width, pos):
    if width == 0:
        return pos

    x = pos % width
    y = pos // width
    odd = y % 2

    return (y*width) + ((width-x-1)*odd) + (x*(1-odd))

for i in range(32):
    print(i, xlat(8, i))

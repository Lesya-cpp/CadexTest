const int icount = 36;
const quint32 indices[icount] = {
0, 3, 1,
1, 3, 2,
4, 7, 5,
5, 7, 6,
8, 11, 9,
9, 11, 10,
12, 15, 13,
13, 15, 14,
16, 19, 17,
17, 19, 18,
20, 23, 21,
21, 23, 22
};

const int vcount = 24;
const vertex vertices[vcount] = {
{{ -size, size, size, 1 }, { 0, 1 }, { 0, 0, 1 } },
{{ size, size, size, 1 }, { 1, 1 }, { 0, 0, 1 } },
{{ size, -size, size, 1 }, { 1, 0 }, { 0, 0, 1 } },
{{ -size, -size, size, 1 }, { 0, 0 }, { 0, 0, 1 } },
{{ size, size, -size, 1 }, { 0, 1 }, { 0, 0, -1 } },
{{ -size, size, -size, 1 }, { 1, 1 }, { 0, 0, -1 } },
{{ -size, -size, -size, 1 }, { 1, 0 }, { 0, 0, -1 } },
{{ size, -size, -size, 1 }, { 0, 0 }, { 0, 0, -1 } },
{{ -size, size, -size, 1 }, { 0, 1 }, { 0, 1, 0 } },
{{ size, size, -size, 1 }, { 1, 1 }, { 0, 1, 0 } },
{{ size, size, size, 1 }, { 1, 0 }, { 0, 1, 0 } },
{{ -size, size, size, 1 }, { 0, 0 }, { 0, 1, 0 } },
{{ size, -size, -size, 1 }, { 0, 1 }, { 0, -1, 0 } },
{{ -size, -size, -size, 1 }, { 1, 1 }, { 0, -1, 0 } },
{{ -size, -size, size, 1 }, { 1, 0 }, { 0, -1, 0 } },
{{ size, -size, size, 1 }, { 0, 0 }, { 0, -1, 0 } },
{{ -size, size, -size, 1 }, { 0, 1 }, { -1, 0, 0 } },
{{ -size, size, size, 1 }, { 1, 1 }, { -1, 0, 0 } },
{{ -size, -size, size, 1 }, { 1, 0 }, { -1, 0, 0 } },
{{ -size, -size, -size, 1 }, { 0, 0 }, { -1, 0, 0 } },
{{ size, size, size, 1 }, { 0, 1 }, { 1, 0, 0 } },
{{ size, size, -size, 1 }, { 1, 1 }, { 1, 0, 0 } },
{{ size, -size, -size, 1 }, { 1, 0 }, { 1, 0, 0 } },
{{ size, -size, size, 1 }, { 0, 0 }, { 1, 0, 0 } }};

import sys
import math
import threading

A, B, C, tab_range = [], [], [], []
ma, mb, na, nb, count, thread_count = 0, 0, 0, 0, 0, 0
global_sum = 0
frobenius_sum = 0
lock = threading.Lock()

def print_matrix(A, m, n):
    print("[");
    for i in range(m):
        for j in range(n):
            print(" ", A[i][j], end = "");
        print("");
    print("]");

def multiply_row(row_num, col_start, col_end):
    global global_sum
    global frobenius_sum
    global lock
    for j in range(col_start, col_end, 1):
        s = 0;
        for k in range(0, na, 1):
            s+=A[row_num][k]*B[k][j];
        C[row_num][j] = s;
        lock.acquire()
        global_sum += s;
        frobenius_sum += s*s;
        lock.release()


def multiply_matrix(f, to):
    row_num = math.floor((f)/nb);
    col_start = f - (row_num*nb);
    col_end = col_start + (to - f) + 1;
    remaining = to - f + 1;
    if(col_end > nb):
        col_end = nb
    while(remaining > 0):
        multiply_row(row_num, col_start, col_end);
        row_num = row_num + 1;
        remaining = remaining -  (col_end - col_start);
        col_start=0;
        col_end=to - remaining + 1;
        if(col_end > nb):
            col_end = nb;
        if(remaining < nb):
            col_end = col_end - 1;

def thread_multiply(id):
    f = tab_range[id];
    to = 0;
    if(id == thread_count - 1):
        to = count - 1;
    else:
        to = tab_range[id+1] - 1;
    multiply_matrix(f, to);

thread_count = int(sys.argv[1])
try:
    fpa = open("A.txt", "r")
    fpb = open("B.txt", "r")
except IOError:
    print("błąd otwarcia pliku")
    exit(-10)

ma = [int(x) for x in next(fpa).split()][0]
na = [int(x) for x in next(fpa).split()][0]
for line in fpa:
    A.append([float(x.strip('.')) for x in line.split()])

mb = [int(x) for x in next(fpb).split()][0]
nb = [int(x) for x in next(fpb).split()][0]
for line in fpb:
    B.append([float(x.strip('.')) for x in line.split()])
if(na != mb):
    print('Złe wymiary')
    EXIT_FAILURE   
count = ma*nb
C = [[0 for x in range(nb)] for y in range(ma)]
for i in range(thread_count):
    dist = math.floor( i * count/thread_count)
    tab_range.append(dist)

threads = []
for i in range(thread_count):
    id = []
    id.append(i)
    t = threading.Thread(target = thread_multiply, args = id)
    t.start()
    threads.append(t)

for thread in threads:
    thread.join()

print("Suma: ", global_sum);
print("C:");
print_matrix(C, ma, nb);
print("Norma Frobeniusa: ", math.sqrt(frobenius_sum));
fpa.close()
fpb.close()
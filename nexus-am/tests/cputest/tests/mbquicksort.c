#include <benchmark.h>

static int N, *data;
Benchmark *current;
Setting *setting;

static char *start;

// Library

void* bench_alloc(size_t size) {
  if ((uintptr_t)start % 16 != 0) {
    start = start + 16 - ((uintptr_t)start % 16);
  }
  char *old = start;
  start += size;
  assert((uintptr_t)_heap.start <= (uintptr_t)start && (uintptr_t)start < (uintptr_t)_heap.end);
  for (char *p = old; p != start; p ++) *p = '\0';
  assert((uintptr_t)start - (uintptr_t)_heap.start <= setting->mlim);
  return old;
}

void bench_free(void *ptr) {
}

void bench_reset() {
  start = (char*)_heap.start;
}

static int32_t seed = 1;

void bench_srand(int32_t _seed) {
  seed = _seed & 0x7fff;
}

int32_t bench_rand() {
  seed = (seed * (int32_t)214013L + (int32_t)2531011L);
  return (seed >> 16) & 0x7fff;
}

void bench_qsort_prepare() {
  bench_srand(1);

  N = setting->size;

  data = bench_alloc(N * sizeof(int));
  for (int i = 0; i < N; i ++) {
    data[i] = (bench_rand() << 16) | bench_rand();
  }
}

static void swap(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

static void myqsort(int *a, int l, int r) {
  if (l < r) {
    int p = a[l], pivot = l, j;
    for (j = l + 1; j < r; j ++) {
      if (a[j] < p) {
        swap(&a[++pivot], &a[j]);
      }
    }
    swap(&a[pivot], &a[l]);
    myqsort(a, l, pivot);
    myqsort(a, pivot + 1, r);
  }
}

void bench_qsort_run() {
  myqsort(data, 0, N);
}

int main(){
  bench_qsort_run();
}

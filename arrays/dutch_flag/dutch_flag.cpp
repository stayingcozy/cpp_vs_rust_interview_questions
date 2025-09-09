#include <vector>
using namespace std;

typedef enum { kRed, kWhite, kBlue } Color;

void DutchFlagPartition(int pivot_index, vector<Color>* A_ptr) {
    vector<Color>& A = *A_ptr;
    Color pivot = A[pivot_index];

    int small = 0, equal = 0, larger = size(A);
    while (equal < larger) {
        if (A[equal] < pivot) {
            swap(A[equal++], A[small++]);
        } else if (A[equal] == pivot) {
            equal++;
        } else { 
            // A[equal] > pivot
            swap(A[equal], A[--larger]);
        }
    }

    return;
}

int main() {
    vector<Color> A {kRed, kWhite, kRed, kBlue, kWhite, kRed, kBlue, kBlue, kWhite, kBlue};
    int pi = 4;

    DutchFlagPartition(pi, &A);

    return 0;
}
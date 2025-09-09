#include <vector>
#include <random>
#include <algorithm>

using namespace std;

void SampleOfflineData(int k, vector<int>* A_ptr) {
    vector<int>& A = *A_ptr;
    default_random_engine seed((random_device())());
    for (int i=0; i<k; i++) {
        swap(A[i], A[uniform_int_distribution<int>{i, static_cast<int>(A.size())-1}(seed)]);
    }

}

int main() {
    vector<int> A = {1, 2, 4, 5, 2};
    SampleOfflineData(3, &A);

    return 0;
}
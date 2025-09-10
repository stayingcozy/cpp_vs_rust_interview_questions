#include <iostream>
#include <vector>
#include <array>

using namespace std;

vector<int> MatrixInSpiralOrder(vector<vector<int>> square_matrix) {
    const array<array<int,2>,4> kShift = {{{0,1}, {1,0}, {0,-1}, {-1,0}}};
    int dir = 0, x=0, y=0;
    vector<int> spiral_order;

    for(int i=0; i < square_matrix.size() * square_matrix.size(); i++) {
        spiral_order.emplace_back(square_matrix[x][y]); // store new loc
        square_matrix[x][y] = 0; // mark as read

        int next_x = x + kShift[dir][0], next_y = y + kShift[dir][1]; // shift in established dir
        if (next_x < 0 || next_x > square_matrix.size() - 1 || next_y < 0 || next_y > square_matrix.size() - 1 || square_matrix[next_x][next_y] == 0) {
            // if lower, upper bounds of x,y then shift to next dir; also if already traveled (new boundary)
            dir = (dir + 1) % 4; // next dir in rotation
            next_x = x + kShift[dir][0], next_y = y + kShift[dir][1]; // update next_x,y
        }
        x = next_x; y = next_y; // update x,y pos to extract spiral pos in next loop

    }
    return spiral_order;
}


int main() {
    vector<vector<int>> square_matrix = {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16}};
    vector<int> spiral = MatrixInSpiralOrder(square_matrix);

    for (int i=0; i<spiral.size(); i++) {
        cout << spiral[i] << "," << endl;
    }
    return 0;
}
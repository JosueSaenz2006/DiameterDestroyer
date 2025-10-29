# DiameterDestroyer

A solution for the "Diameter Problem Again" - an advanced tree diameter query problem with dynamic list operations and encrypted queries.

## Problem Description

Given a weighted tree with N nodes (weights can be negative), process M operations on a dynamic list A:
1. **Type 1**: Append a node to list A
2. **Type 2**: Remove the last node from A
3. **Type 3**: Find the node in A[l..r] that is farthest from node x

Query parameters are encrypted using XOR with the absolute value of the last answer.

## Algorithm

The solution uses:
- **LCA (Lowest Common Ancestor)** with Euler tour and sparse table RMQ for O(1) distance queries
- **Segment Tree** maintaining diameter endpoints for efficient range queries
- **Diameter Property**: For any set S with diameter endpoints (a,b), the farthest node from any x is either a or b

Time Complexity: O((N + M) log M) with O(N log N) preprocessing

## Compilation

```bash
g++ -std=c++17 -O2 solution.cpp -o solution
```

## Usage

```bash
./solution < input.txt
```

### Input Format

```
N
u1 v1 w1
u2 v2 w2
...
u(N-1) v(N-1) w(N-1)
M
operation1
operation2
...
operationM
```

Where operations are:
- `1 x` - Append node x
- `2` - Remove last node
- `3 l r x` - Query farthest node in A[l..r] from x

### Example

```
3
1 2 10
2 3 -15
5
1 1
1 2
1 3
3 1 3 1
3 1 3 3
```

Output:
```
0
10
```

## Testing

Multiple test cases are provided to verify the solution:

- **test1.txt**: Basic operations with encryption
- **test2.txt**: Linear tree with sequential queries
- **test3.txt**: Tree with negative weights
- **test_encrypted.txt**: Tests XOR encryption logic
- **test_complex.txt**: Star tree with multiple branches

Run all tests:
```bash
make test
```

Or run individual tests:
```bash
./solution < test1.txt
```

## Implementation Details

- Handles negative edge weights correctly
- Encrypts/decrypts query parameters using XOR
- Empty list queries return 0
- Efficient O(log M) per operation after O(N log N) preprocessing
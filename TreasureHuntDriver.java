import java.util.Arrays;

public class TreasureHuntDriver {
    public static void main(String[] args) {
        TreasureHunt treasureHunt = new TreasureHunt();

        // Define 5 test cases with different grid configurations
        int[][][] testCases = {
            {  // Test Case 1
                {3, 2, 5},
                {1, 9, 8},
                {4, 6, 2}
            },
            {  // Test Case 2
                {10, 7, 8, 12},
                {4, 2, 5, 15},
                {1, 9, 3, 10},
                {6, 13, 11, 8}
            },
            {  // Test Case 3
                {1, 2, 3, 4},
                {5, 6, 7, 8},
                {9, 10, 11, 12},
                {13, 14, 15, 16}
            },
            {  // Test Case 4
                {5, 9, 1, 4, 2},
                {3, 8, 7, 6, 5},
                {1, 2, 3, 4, 5},
                {9, 8, 7, 6, 5},
                {1, 2, 3, 4, 5}
            },
            {  // Test Case 5
                {3},
                {2},
                {5},
                {1},
                {4}
            }
        };

        // Loop through test cases
        for (int i = 0; i < testCases.length; i++) {
            int[][] grid = testCases[i];
            int rows = grid.length;
            int cols = grid[0].length;

            System.out.println("============================================");
            System.out.println("Running Test Case " + (i + 1) + " (Grid Size: " + rows + "x" + cols + ")");
            printGrid(grid);

            // Recursive Approach
            long startRecursive = System.nanoTime();
            int minRiskRecursive = treasureHunt.findMinRiskRecursive(grid, 0, 0);
            long endRecursive = System.nanoTime();
            System.out.println("Minimum Risk (Recursive): " + minRiskRecursive);
            // System.out.println("Execution Time (Recursive): " + (endRecursive - startRecursive) / 1e6 + " ms\n");

            // Memoization Approach
            int[][] memo = new int[rows][cols];
            for (int[] row : memo) Arrays.fill(row, -1);
            long startMemoization = System.nanoTime();
            int minRiskMemoization = treasureHunt.findMinRiskMemoization(grid, 0, 0, memo);
            long endMemoization = System.nanoTime();
            System.out.println("Minimum Risk (Memoization): " + minRiskMemoization);
            // System.out.println("Execution Time (Memoization): " + (endMemoization - startMemoization) / 1e6 + " ms\n");

            // Tabulation Approach
            long startTabulation = System.nanoTime();
            int minRiskTabulation = treasureHunt.findMinRiskTabulation(grid);
            long endTabulation = System.nanoTime();
            System.out.println("Minimum Risk (Tabulation): " + minRiskTabulation);
            // System.out.println("Execution Time (Tabulation): " + (endTabulation - startTabulation) / 1e6 + " ms\n");
        }
    }

    // Function to print the grid
    private static void printGrid(int[][] grid) {
        System.out.println("Grid:");
        for (int[] row : grid) {
            for (int value : row) {
                System.out.print(value + "\t");
            }
            System.out.println();
        }
        System.out.println();
    }
}

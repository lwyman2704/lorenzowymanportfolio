//Lorenzo Wyman
//10/24/2025
//Dr.Ali
//COP3503C Fall 2025

public class TreasureHunt {

    public int findMinRiskRecursive(int [][] grid, int row, int col){

        if (grid == null || grid.length == 0){
            return 0;
        }

        int n = grid.length; //the length going on x axis
        int m = grid[0].length; //the num of columns in row 1

        if (row == n - 1 && col == m - 1){

            return grid[row][col];
        }

        if (row >= n || col >= m){ //we are out of bounds
            return Integer.MAX_VALUE;
        }

        int totalRisk;
        int rightRisk = findMinRiskRecursive(grid, row, col + 1);
        int downRisk = findMinRiskRecursive(grid, row + 1, col);

        totalRisk = grid[row][col] + Math.min(downRisk, rightRisk);

        return totalRisk;

    }

    public int findMinRiskMemoization(int [][] grid, int row, int col, int[][] memo){

         if (grid == null || grid.length == 0){
            return 0;
        }

        int n = grid.length; //the length going on x axis
        int m = grid[0].length; //the num of columns in row 1

        if (row >= n || col >= m){
            return Integer.MAX_VALUE; //were out of bounds
        }

        if (row == n-1 && col == m-1){ //we are at (n-1, m-1) the end
            return grid[row][col];
        }

        if (memo[row][col] != -1){ //if we already been there or computed value

            return memo[row][col];
        }

        int totalRisk;
        int downRisk = findMinRiskMemoization(grid, row + 1, col, memo);
        int rightRisk = findMinRiskMemoization(grid, row, col + 1, memo);

        totalRisk = grid[row][col] + Math.min(downRisk, rightRisk);
        memo[row][col] = totalRisk;

        return totalRisk;

    }

    public int findMinRiskTabulation(int[][] grid){

        if (grid == null || grid.length == 0){
            return 0;
        }

        int n = grid.length; //the length going on x axis
        int m = grid[0].length; //the num of columns in row 1

        int newGrid[][] = new int[n][m]; //make new grid like in class but stores min risk values

        for (int i = 0; i < n; i++){

            for (int j = 0; j < m; j++){

                if (i == 0 && j == 0){
                    newGrid[0][0] = grid[0][0]; //store risk from 0,0 to n-1,m-1 but make equal to the other grid

                }

                else if (i == 0 && j > 0){
                    newGrid[0][j] = grid[0][j] + newGrid[0][j-1]; //on first row but only can come by moving right not above
                    
                }

                else if (i > 0 && j == 0){
                    newGrid[i][0] = grid[i][0] + newGrid[i - 1][0]; //on first row but only can come by moving down
                    
                }

                else {
                    int aboveRisk = newGrid[i - 1][j]; //the row above
                    int leftRisk = newGrid[i][j - 1]; //column to left

                    newGrid[i][j] = grid[i][j] + Math.min(aboveRisk, leftRisk);
                }
            }
        }

        return newGrid[n-1][m-1];
    }

}

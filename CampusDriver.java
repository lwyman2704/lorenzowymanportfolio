import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class CampusDriver {
    public static void main(String[] args) {
        Scanner input = new Scanner(System.in);

        // Ask for the input file name
        System.out.print("Enter input file name: ");
        String fileName = input.nextLine();

        // Count the number of lines (connections) in the input file
        int connectionCount = 0;
        try {
            Scanner fileScanner = new Scanner(new File(fileName));
            while (fileScanner.hasNextLine()) {
                fileScanner.nextLine();
                connectionCount++;
            }
            fileScanner.close();
        } catch (FileNotFoundException e) {
            System.out.println("Error: File not found.");
            return;
        }

        // Create instance of student class and build network
        try {
            CampusNetworkPlanner planner = new CampusNetworkPlanner(connectionCount, fileName);
            String result = planner.buildNetwork();
            System.out.println("\nSelected Connections:");
            System.out.println(result);
        } catch (Exception e) {
            System.out.println("Runtime error: " + e.getMessage());
        }
    }
}

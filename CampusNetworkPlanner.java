//Lorenzo Wyman
//Assignment 5 
//COP3503C
//Dr. Ali
//11/12/2025 @6pm

import java.io.File;
import java.util.Scanner;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Collections;
import java.io.FileNotFoundException;
import java.util.*;

class Edge implements Comparable<Edge> {

    int u;
    int v;
    double cost;

    public Edge(int u, int v, double cost){

        this.u = u;
        this.v = v;
        this.cost = cost;
    }

    @Override
    public int compareTo(Edge other){
        return Double.compare(this.cost, other.cost);
    }

}

public class CampusNetworkPlanner {
    
    private String fileName;
    private String[] idToNames;

    public int numConnections;
    public int numOfCampuses;

    private ArrayList<Edge> totalEdges; //holds all edges
    private HashMap<String, Integer> nameToId; //plan to map names to number ids

    public class UnionFind {

        private int[] parent;
        private int[] rank;

    
        public UnionFind(int n){ //helper to check for cycles

            this.parent = new int[n];
            this.rank = new int[n];

            for (int i = 0; i < n; i++){

                parent[i] = i;
                rank[i] = 0;
            }

        }

        public int find(int x){ //helper finding if they are in the same set
            if (parent[x] != x){
                parent[x] = find(parent[x]);
            }
            return parent[x];
        }

        public boolean unionOperations(int x, int y){ //helper to combine the sets

            int xRoot = find(x);
            int yRoot = find(y);

            if (xRoot == yRoot){ //if this is a cycle
                return false;

            }

            if (rank[xRoot] < rank[yRoot]){
                parent[xRoot] = yRoot; //attatch small roo to larger tree

            } else if (rank[xRoot] > rank[yRoot]){
                parent[yRoot] = xRoot;  

            } else {

                parent[yRoot] = xRoot;
                rank[xRoot]++;
            }

            return true;
        }
    }

    public CampusNetworkPlanner(int numConnections, String name){ //constructor

        this.numConnections = numConnections;
        this.fileName = name;
        this.numOfCampuses = 0;

        this.totalEdges = new ArrayList<>();
        this.nameToId = new HashMap<>();
        
    }


    public String buildNetwork(){

        //select connections needed t build network
        //no cycles check
        //return sorted string of campus names connections
        //total cost formatted too

        try {

            //reading and parse file
            Scanner fs = new Scanner (new File(this.fileName));
            while (fs.hasNextLine()){

                String line = fs.nextLine();
                String[] pieces = line.split(" ");
                //parse pieces (campusx, campusY, $cost)
                String campusX = pieces[0];
                String campusY = pieces[1];
                double cost = Double.parseDouble(pieces[2]); 

                //map names to ids with hasmap if they dont alr have a key
                if(!nameToId.containsKey(campusX)){
                    nameToId.put(campusX, numOfCampuses++);

                } if ( !nameToId.containsKey(campusY)){
                    nameToId.put(campusY, numOfCampuses++);
                }

                int id1 = nameToId.get(campusX);
                int id2 = nameToId.get(campusY);
                totalEdges.add(new Edge(id1, id2, cost)); //puts them in the edge list
            }
            fs.close();

        } catch (FileNotFoundException e){

            System.out.println("File doesnt exist" + e.getMessage());
            return "file error";
        }

        idToNames = new String[numOfCampuses]; //reverse mapping for final output
        for (String name : nameToId.keySet()){
            int id = nameToId.get(name);
            idToNames[id] = name;
        }

        Collections.sort(totalEdges); //sort edges by cost
        UnionFind uf = new UnionFind(numOfCampuses); //initialize union find structure

        double overallCost = 0.0;
        ArrayList<String> resultantStrings = new ArrayList<>();

        for (Edge edge : totalEdges){ //traverse sorted edges, add them if they arent cycles 
            if (uf.unionOperations(edge.u, edge.v)){
                overallCost += edge.cost;
                
                String campusNameX = idToNames[edge.u]; //get names from ids so we can format
                String campusNameY = idToNames[edge.v];

                if (campusNameX.compareTo(campusNameY) < 0){ //formatting with String instead of (int) cuz it errored
                    resultantStrings.add(campusNameX + "---" + campusNameY + "\t$" + String.format("%.0f", edge.cost));
                } else {
                    resultantStrings.add(campusNameY + "---" + campusNameX + "\t$" + String.format("%.0f", edge.cost));
                }
            }


        }

        Collections.sort(resultantStrings); //sort final result strings
        StringBuilder finalResult = new StringBuilder();

        for (String s : resultantStrings){ //sort and build final string
            finalResult.append(s).append("\n");

        }
        finalResult.append("\nTotal cost: $").append((int)overallCost);

        return finalResult.toString();
    }

}

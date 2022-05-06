#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

//Print path recursively
void path(int parent[], int n)
{
   if(parent[n] == -1)
       return;
       
   path(parent, parent[n]);
   printf("->%d", n+1);

 }

//To print the path for every  vertex
void print_path(int V, int parent[], int dist[], int src)
{
      int i,j;   
      for(i=0; i<V; i++)
      {
          if(i!=src)
          {
             if(dist[i] == INT_MAX)
                {
                   printf("No path\t\t\t\t INFINTY\n");
                }
             else
             {
             j = i;
             printf("%d", src+1);
             path(parent, j);
             printf("\t\t\t\t %d\n", dist[i]);
             }
          }
           
          else
              printf("%d->%d\t\t\t\t 0\n", i+1, i+1);
          
      }
}

//Algorithm to find shortest path
void Dijkstra(int V, int G[V][V], int src)
{
    //declare required arrays for distance, visited, nodes
    int parent[V], i, j, k, dist[V];
    
    //Visited array to check if particular node is visited
    bool visited[V];
    
   //Initialize the values
   for(i=0; i<V; i++)
   {
      dist[i] = INT_MAX;
      visited[i] = false;
      parent[i] = -1;
   }
   
   //self distance will be zero
   dist[src] = 0;
   
   
   //Now find shortest path for each from src;
   for(i=0; i<V-1; i++)
   { 
      //find the minimum distance value and its index
      int min_value = INT_MAX, min_index;
        for(j=0; j<V; j++)
        {
           if(visited[j] == false && dist[j] < min_value)
              {
                 min_value = dist[j];
                 min_index = j;
              }
       }
       
       //Markthat node is visited
       visited[min_index] = true;
       
       //Update the distance of the adjacent nodes of it
       for(k=0; k<V; k++)
       {
         if(visited[k] == false && G[min_index][k] && (dist[min_index] + G[min_index][k] <= dist[k]))
         {
            parent[k] = min_index;
            dist[k] = dist[min_index] + G[min_index][k];
         }       
       }
    }
      //print the path 
      print_path(V,parent, dist,src);  
}

int main(){
    int m,n,u,v,i,j,w;
    //Get the nodes and its links number
    //printf("Enter the number of nodes and links:\n");
    scanf("%d %d", &n, &m);
    
    //declare graph G and initilise with zero
    int G[n][n];
    memset(G, 0, sizeof(G[0][0])*n*n);
    
    //Give self distance to 0
    for(i=0; i<n; i++)
        G[i][i] = 0;
     
    //Get the weights for each link
    //printf("Enter the links\n");
       
    for(i=0; i<m; i++)
       {
        scanf("%d %d %d", &u, &v, &w);
        G[u-1][v-1] = w;
        G[v-1][u-1] = w; 
       }
      
      printf("\n");
       
     //get shortest for each node with Dijkstra's algorithm
     for(i=0; i<n; i++){
       
       printf("From Node: %d\n", i+1);
       printf("Path\t\t\t\t Distance\n");
        Dijkstra(n, G, i);
        printf("\n");
        
    }
     
return 0;
}


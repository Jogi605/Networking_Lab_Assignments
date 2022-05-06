#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#define MAX 10000

//function to print routing table for each node
void print_path(int V, int dist[V][V], int parent[V][V]){

    int i, j;
    
    //print the path
    for(i=0; i<V; i++){
       
       printf("From Node: %d\n", i+1);
       printf("Destination\tNext_hop\tCost\n");
       
       for(j=0; j<V; j++){
             printf("%d\t\t%d\t\t%d\n", j+1, parent[i][j]+1, dist[i][j]);
             }
       printf("\n");
       }
        
}

//Algorithm to find shortest path for each node
void Bellman_Ford(int V, int dist[V][V]){
  int i, j, k, parent[V][V];
  
  //Intialise the parent values with src indexes
  for(i=0; i<V; i++){
    for(j=0; j<V; j++){
        parent[i][j] = j;
    }
  }
  
  //Update paths for each node
  for(i=0; i<V; i++)
  {
    
    for(j=0; j<V; j++)
    {
      for(k=0; k<V; k++)
      {
       
        //If the distance is more than the previous, change and update its parent
        if(dist[i][j]> dist[i][k] + dist[k][j])
          {
          
           dist[i][j] = dist[k][j] + dist[i][k];
           parent[i][j] = k;
           
          }
       }
     }
   }
   
   //print the path now  
   print_path(V, dist, parent); 
  }
  
       
int main(){
    int m,n,u,v,i,j,w;
    //Get the nodes and its links number
    //printf("Enter the number of nodes and links:\n");
    
    scanf("%d %d", &n, &m);
    
    //declare graph G and initilise with zero
    int G[n][n];
    //memset(G, MAX, sizeof(G[0][0])*n*n);
    
    //Initialise the Graph 
    for(i=0; i<n; i++)
    {
     for(j=0; j<n; j++)
     {
     
        G[i][j] = MAX;
        
     }
    }
    
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
        G[u-1][u-1] = 0;
        G[v-1][v-1] = 0;
       }
      
    //Call Bellman_ford algorithm to find shortest path  
    Bellman_Ford(n, G);
       
return 0;
}


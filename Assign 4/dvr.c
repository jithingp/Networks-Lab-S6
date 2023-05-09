#include<stdio.h>
#include<limits.h>
#include<stdlib.h>

const int imax = 10000;

void bellmanji(int src,int n,int m,int mat[100][100]){
    int next[100];
    int dist[100];
    int boss[100];

    for(int i=0;i<100;i++){
        dist[i]=imax;
        boss[i]=-1;
        next[i]=-1;
    }

    dist[src]=0;
    next[src]=src;

    for(int i=0;i<n-1;i++){
        for(int j=1;j<=n;j++){
            for(int k=1;k<=n;k++){
                if(mat[j][k]==imax || dist[j]==imax)
                    continue;

                if(dist[j]+mat[j][k] < dist[k]){
                    dist[k]=dist[j]+mat[j][k];
                    boss[k]=j;
                }
            }
        }
    }

    for(int j=1;j<=n;j++){
        for(int k=1;k<=n;k++){
            if(mat[j][k]==imax || dist[j]==imax)
                continue;

            if(dist[j]+mat[j][k] < dist[k]){
                printf("Contains negative cycle...\n");
                printf("%d %d %d\n",src,j,k);//check
                printf("%d %d %d\n",dist[j],mat[j][k],dist[k]);//check
                exit(0);
            }
        }
    }

    for(int i=1;i<=n;i++){
        if(boss[i]==-1)
            next[i]=i;
        else{    
            int j=i;
            while(boss[j]!=src){
                j=boss[j];
            }
            next[i]=j;
        }
    }

    printf("Distance vector table for Node %d : \n\n",src);
    printf("Destination  Next  Cost\n\n");

    for(int i=1;i<=n;i++){
        printf(" | %d\t\t  %d\t\t%d |\n",i,next[i],dist[i]);
    }


}
int main(){
    int n,m,u,v,w;
    scanf("%d %d",&n,&m);
    int mat[100][100];

    for(int i=0;i<100;i++){
        for(int j=0;j<100;j++){
            mat[i][j]=imax;
        }
    }

    for(int i=0;i<m;i++){

        scanf("%d %d %d",&u,&v,&w);
        mat[u][v]=w;
        mat[v][u]=w;
    }
    for(int i=1;i<=n;i++){
        printf("------------------------\n");
        bellmanji(i,n,m,mat);
    }
    printf("------------------------");
}
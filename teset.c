#include <stdio.h>
#include <stdlib.h>

double **data;
int *clusters;
int *visited;
int *neigh_points;

int FEATURES;
int DATASET_SIZE; // should change accourding to actual frame
float EPSILON;
int MIN_POINTS;

void loadData(char file_name[])
{
    data = (double**)calloc(DATASET_SIZE, sizeof(double*));
    
    visited = (int*)calloc(DATASET_SIZE, sizeof(int));
    
    neigh_points = (int*)calloc(DATASET_SIZE*DATASET_SIZE, sizeof(int));
    
    clusters = (int*)calloc(DATASET_SIZE, sizeof(int));
    
    int i, j;
    
    for(i = 0; i < DATASET_SIZE; i++)
    {
        data[i] = (double*)calloc(FEATURES, sizeof(double));
    }
    
    FILE *fp;
    fp = fopen(file_name, "r");
    
    for(i = 0; i < DATASET_SIZE; i++)
    {
        for(j = 0; j < FEATURES; j++)
        {
            fscanf(fp, "%lf", &data[i][j]);
        }
    }
    
    fclose(fp);
}

int regionQuery(int start, int index)
{
    int i, j, count = 0;
    double distance, temp;
    for(i = 0; i < DATASET_SIZE; i++)
    {
        if(i != index)
        {
            distance = 0;
            for(j = 0; j < FEATURES; j++)
            {
                temp = data[i][j] - data[index][j];
            
                distance += temp * temp; // each point's distance to the index
            }
            if(distance <= EPSILON)
            {
                neigh_points[start+count] = i; // the position index of points in the epsilon region of the data set
                count++; // stands for the number of points in the epsilon region
            }
        }
    }
    return count;
}

void printData(int f)
{
    int i, j;
    char framename[20];
    FILE *fp;
    sprintf(framename, "#%dDbframe.txt", f);
    fp = fopen(framename, "w"); // need to change to match the frame number
    
    for(i = 0; i < DATASET_SIZE; i++)
    {
        //fprintf(fp, "%d\n", clusters[i]);
        //if (clusters[i] != 0)
        //{
            fprintf(fp, "%d\n", clusters[i]);
        //}
    }
}

void expandCluster(int cluster_no, int num_npoints, int index)
{
    clusters[index] = cluster_no;
    
    int i, count = 0;
    
    for(i = 0; i < num_npoints; i++)
    {
        if(!visited[neigh_points[i]])
        {
            visited[neigh_points[i]] = 1;
            
            count = regionQuery(num_npoints, neigh_points[i]); // searching surrounding points
            
            if(count >= MIN_POINTS)
            {
                num_npoints += count;
            }
        }
        
        if(!clusters[neigh_points[i]])
        {
            clusters[neigh_points[i]] = cluster_no;
        }
    }
}

void dbScan()
{
    int next_cluster = 1, i, j, num_npoints;
    
    for(i = 0; i < DATASET_SIZE; i++)
    {
        if(!visited[i])
        {
            visited[i] = 1;
            
            num_npoints = regionQuery(0, i);
            
            if(num_npoints > MIN_POINTS)
            {
                expandCluster(next_cluster, num_npoints, i); // num_npoints: number of points in epsilon region
                
                next_cluster++;
            }
        }
    }
}   

int rows(int num)
{
    // to decide the number of lines in one frame
    char filename[20];
    sprintf(filename, "#%dframe.txt", num);
    FILE *f = fopen(filename, "r");
    char buffer[384*4];
    int count = 0;
    if(f == NULL) 
    {
        perror("Error opening file.");
    }
    else 
    {
        while(fgets(buffer, sizeof(buffer), f)) 
        {
            count = count + 1;
        }
    }
    fclose(f);
    return count;
} 

int main()
{
    int f = 2;
    FEATURES = 3;
    DATASET_SIZE = rows(f); // should change accourding to actual frame
    EPSILON = 0.5 * 0.5;
    MIN_POINTS = 30;
    char framename[20];
    sprintf(framename, "#%dframe.txt", f);
    loadData(framename);
    dbScan();
    printData(f);
    printf("%lf %d\n", data[10][0], DATASET_SIZE);
}






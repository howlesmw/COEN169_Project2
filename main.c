/*

Stephanie Cervi
COEN 169: Web Information Management
Project 2: Recommendation Systems

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

// Global variables
int ratings[500][1000];
int ratingResults[100][1000];
int testNumber;
int userOffset;

// Function prototypes
int pearsonStandard (int, int);
int pearsonInverse (int, int);
int pearsonCase (int, int);
int vector (int, int);
int cosineSimilarity (int, int);
int custom (int, int);

void calculate (void)
{
	// Initialize variables for use in functions
	int i, j, rating;
	i = 0;
	j = 0;

	// Ask user which algorithm to use
	int algorithm;
	printf ("\nWhich algorithm would you like to use to predict ratings?\n");
	printf ("1 - Pearson Correlation\n");
	printf ("2 - Vector Similarity\n");
	printf ("3 - Pearson Correlation - Inverse User Frequency\n");
	printf ("4 - Pearson Correlation - Case Modification\n");
	printf ("5 - Cosine Similarity\n");
	printf ("6 - Custom Algorithm\n");
	scanf ("%d", &algorithm);

	// Iterate through array to find ratings that need to be calculated
	for (i=userOffset; i<userOffset+100; i++)
	{
		for (j=0; j<1000; j++)
		{
			if (ratings[i][j] == 0)
			{
				int user = i;
				int movie = j;

				// Call the appropriate calculation function
				switch (algorithm)
				{
					case 1:
						rating = pearsonStandard (user, movie);
						break;
						
					case 2:
						rating = vector (user, movie);
						break;
						
					case 3:
						rating = pearsonInverse (user, movie);
						break;
						
					case 4:
						rating = pearsonCase (user, movie);
						break;
						
					case 5:
						rating = cosineSimilarity (user, movie);
						break;
						
					case 6:
						rating = custom (user, movie);
						break;
					
					default:
						rating = pearsonStandard (user, movie);
						break;
				}
				ratingResults[user-userOffset][movie] = rating;
			}
		}
	}
}

int pearsonStandard (int user, int movie)
{
	// Declare variables for use in function
	int rating = 0;
	int testMovies[testNumber];
	float relevantUsers[200][4]; // 0: average - 1: std dev - 2: sim. weight - 3: user ID
	float userAverage = 0;
	float userStdDev = 0;
	int i, j, count, relevantCount;
	
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
		{
			relevantUsers[i][j] = 0;
		}
	}

	// Calculate average rating for user in question
	count = 0;
	userAverage = 0;
	for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9)) // test for values within range of valid ratings
		{
			testMovies[count] = i;
			userAverage += ratings[user][i];
			count++;
		}
	}
	userAverage = userAverage / (float)count;
	relevantCount = 0;
	for (i=0; i<200; i++)
	{
		count = 0;
		for (j=0; j<testNumber; j++) // do we even need to look at this user?
		{
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
			{
				count++;
			}
		}
		if (count == testNumber) // if the user has rated the same movies, then we look at their data
		{
			for (j=0; j<testNumber; j++)
			{
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
					relevantUsers[relevantCount][3] = i;
					relevantUsers[relevantCount][0] += ratings[i][j];
				}
			}
			relevantUsers[relevantCount][0] = relevantUsers[relevantCount][0] / count;
			relevantCount++;
		}
	}

	// Compute denominator elements of similarity weight
	for (i=0; i<testNumber; i++)
	{
		userStdDev += (ratings[user][testMovies[i]] - userAverage) * (ratings[user][testMovies[i]] - userAverage);
	}
	userStdDev = sqrt (userStdDev);
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
					relevantUsers[i][1] += (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			}
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}

	// Compute similarity weight
	float numerator, denominator;
	numerator = 0;
	denominator = 0;
	
	if (relevantCount == 0)
	{
		rating = userAverage;
		return rating;
	}
	
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
				numerator += (ratings[user][testMovies[j]] - userAverage) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			}
			denominator = userStdDev * relevantUsers[i][1];
			relevantUsers[i][2] = numerator / denominator;
		}
	}

	// Compute prediction
	numerator = 0;
	denominator = 0;
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{

				numerator += relevantUsers[i][2] * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
				denominator += fabsf(relevantUsers[i][2]);
			}
		}
	}
	rating = userAverage + (numerator / denominator) + 0.5;
	
	if (rating > 5)
	{
		rating = 5;
	}
	if (rating < 1)
	{
		rating = 1;
	}
	
	// Return final calculated value
	return rating;	
}

int pearsonInverse (int user, int movie)
{
    // Declare variables for use in function
	int rating = 0;
	int testMovies[testNumber];
    float iufs[testNumber];
    int usersRated;
    int totalUsers = 200;
	float relevantUsers[200][4]; // 0: average - 1: std dev - 2: sim. weight - 3: user ID
	float userAverage = 0;
	float userStdDev = 0;
	int i, j, count, relevantCount;
	
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
		{
			relevantUsers[i][j] = 0;
		}
	}
    
    // Find relevant movies
    count = 0;
    for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9))
		{
			testMovies[count] = i;
            count++;
        }
    }
    
    // Calculate IUF for each relevant movie
    for (i=0; i<testNumber; i++)
    {
        usersRated = 0;
        for (j=0; j<200; j++)
        {
            if ((ratings[j][testMovies[i]] != 0) && (ratings[j][testMovies[i]] != 9))
            {
                usersRated++;
            }
        }
        iufs[i] = log ((float)totalUsers / (float)usersRated);
    }
    
	// Calculate average rating for user in question
	count = 0;
	userAverage = 0;
	for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9)) // test for values within range of valid ratings
		{
			userAverage += ratings[user][i] * iufs[count];
			count++;
		}
	}
	userAverage = userAverage / (float)count;
    
	relevantCount = 0;
	for (i=0; i<200; i++)
	{
		count = 0;
		for (j=0; j<testNumber; j++) // do we even need to look at this user?
		{
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
			{
				count++;
			}
		}
		if (count == testNumber) // if the user has rated the same movies, then we look at their data
		{
			for (j=0; j<testNumber; j++)
			{
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
					relevantUsers[relevantCount][3] = i;
					relevantUsers[relevantCount][0] += ratings[i][j] * iufs[relevantCount];
				}
			}
			relevantUsers[relevantCount][0] = relevantUsers[relevantCount][0] / count;
			relevantCount++;
		}
	}
    
	// Compute denominator elements of similarity weight
	for (i=0; i<testNumber; i++)
	{
		userStdDev += ((ratings[user][testMovies[i]] * iufs[i]) - userAverage) * ((ratings[user][testMovies[i]] * iufs[i]) - userAverage);
	}
	userStdDev = sqrt (userStdDev);
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
                relevantUsers[i][1] += ((ratings[(int)relevantUsers[i][3]][testMovies[j]] * iufs[j]) - relevantUsers[i][0]) * ((ratings[(int)relevantUsers[i][3]][testMovies[j]] * iufs[j]) - relevantUsers[i][0]);
			}
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}
    
	// Compute similarity weight
	float numerator, denominator;
	numerator = 0;
	denominator = 0;
	
	if (relevantCount == 0)
	{
		rating = userAverage + 0.5;
        
        if (rating > 5)
        {
            rating = 5;
        }
        if (rating < 1)
        {
            rating = 1;
        }
        
		return rating;
	}
	
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
				numerator += ((ratings[user][testMovies[j]] * iufs[j]) - userAverage) * ((ratings[(int)relevantUsers[i][3]][testMovies[j]] * iufs[j]) - relevantUsers[i][0]);
			}
			denominator = userStdDev * relevantUsers[i][1];
			relevantUsers[i][2] = numerator / denominator;
		}
	}
    
	// Compute prediction
	numerator = 0;
	denominator = 0;
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
                
				numerator += relevantUsers[i][2] * ((ratings[(int)relevantUsers[i][3]][testMovies[j]] * iufs[j]) - relevantUsers[i][0]);
				denominator += fabsf(relevantUsers[i][2]);
			}
		}
	}
	rating = userAverage + (numerator / denominator) + 0.5;
	
	if (rating > 5)
	{
		rating = 5;
	}
	if (rating < 1)
	{
		rating = 1;
	}
	
	// Return final calculated value
	return rating;
}

int pearsonCase (int user, int movie)
{
	// Declare variables for use in function
	int rating = 0;
	int testMovies[testNumber];
	float relevantUsers[200][4]; // 0: average - 1: std dev - 2: sim. weight - 3: user ID
	float userAverage = 0;
	float userStdDev = 0;
	int i, j, count, relevantCount;
	
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
		{
			relevantUsers[i][j] = 0;
		}
	}
    
	// Calculate average rating for user in question
	count = 0;
	userAverage = 0;
	for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9)) // test for values within range of valid ratings
		{
			testMovies[count] = i;
			userAverage += ratings[user][i];
			count++;
		}
	}
	userAverage = userAverage / (float)count;
	relevantCount = 0;
	for (i=0; i<200; i++)
	{
		count = 0;
		for (j=0; j<testNumber; j++) // do we even need to look at this user?
		{
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
			{
				count++;
			}
		}
		if (count == testNumber) // if the user has rated the same movies, then we look at their data
		{
			for (j=0; j<testNumber; j++)
			{
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
					relevantUsers[relevantCount][3] = i;
					relevantUsers[relevantCount][0] += ratings[i][j];
				}
			}
			relevantUsers[relevantCount][0] = relevantUsers[relevantCount][0] / count;
			relevantCount++;
		}
	}
    
	// Compute denominator elements of similarity weight
	for (i=0; i<testNumber; i++)
	{
		userStdDev += (ratings[user][testMovies[i]] - userAverage) * (ratings[user][testMovies[i]] - userAverage);
	}
	userStdDev = sqrt (userStdDev);
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
                relevantUsers[i][1] += (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			}
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}
    
	// Compute similarity weight
	float numerator, denominator;
	numerator = 0;
	denominator = 0;
	
	if (relevantCount == 0)
	{
		rating = userAverage;
		return rating;
	}
	
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
				numerator += (ratings[user][testMovies[j]] - userAverage) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			}
			denominator = userStdDev * relevantUsers[i][1];
			relevantUsers[i][2] = numerator / denominator;
            relevantUsers[i][2] = relevantUsers[i][2] * abs(pow (relevantUsers[i][2], 1.5));
		}
	}
    
	// Compute prediction
	numerator = 0;
	denominator = 0;
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
                
				numerator += relevantUsers[i][2] * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
				denominator += fabsf(relevantUsers[i][2]);
			}
		}
	}
	rating = userAverage + (numerator / denominator) + 0.5;
	
	if (rating > 5)
	{
		rating = 5;
	}
	if (rating < 1)
	{
		rating = 1;
	}
	
	// Return final calculated value
	return rating;	
}

int vector (int user, int movie)
{
	// Declare variables for use in function
	int rating = 0;
	int testMovies[testNumber];
	float relevantUsers[200][4]; // 0: average - 1: std dev - 2: sim. weight - 3: user ID
	float userAverage = 0;
	float userStdDev = 0;
	int i, j, count, relevantCount;
	
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
		{
			relevantUsers[i][j] = 0;
		}
	}
    
	// Calculate average rating for user in question
	count = 0;
	userAverage = 0;
	for (i=0; i<1000; i++)
	{
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9)) // test for values within range of valid ratings
		{
			testMovies[count] = i;
			userAverage += ratings[user][i];
			count++;
		}
	}
	userAverage = userAverage / (float)count;
	relevantCount = 0;
	for (i=0; i<200; i++)
	{
		count = 0;
		for (j=0; j<testNumber; j++) // do we even need to look at this user?
		{
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
			{
				count++;
			}
		}
		if (count == testNumber) // if the user has rated the same movies, then we look at their data
		{
			for (j=0; j<testNumber; j++)
			{
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
					relevantUsers[relevantCount][3] = i;
					relevantUsers[relevantCount][0] += ratings[i][j];
				}
			}
			relevantUsers[relevantCount][0] = relevantUsers[relevantCount][0] / count;
			relevantCount++;
		}
	}
    
	// Compute denominator elements of similarity weight
	for (i=0; i<testNumber; i++)
	{
		userStdDev += (ratings[user][testMovies[i]] - userAverage) * (ratings[user][testMovies[i]] - userAverage);
	}
	userStdDev = sqrt (userStdDev);
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
                relevantUsers[i][1] += (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			}
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}
    
	// Compute similarity weight
	float numerator, denominator;
	numerator = 0;
	denominator = 0;
	
	if (relevantCount == 0)
	{
		rating = userAverage;
		return rating;
	}
	
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
				numerator += (ratings[user][testMovies[j]] - userAverage) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			}
			denominator = userStdDev * relevantUsers[i][1];
			relevantUsers[i][2] = numerator / denominator;
		}
	}
    
	// Compute prediction
	numerator = 0;
	denominator = 0;
	for (i=0; i<200; i++)
	{
		if (relevantUsers[i][3] != 0)
		{
			for (j=0; j<testNumber; j++)
			{
                
				numerator += relevantUsers[i][2] * ratings[(int)relevantUsers[i][3]][testMovies[j]];
				denominator += relevantUsers[i][2];
			}
		}
	}
	rating = (numerator / denominator) + 0.5;
	
	if (rating > 5)
	{
		rating = 5;
	}
	if (rating < 1)
	{
		rating = 1;
	}
	
	// Return final calculated value
	return rating;	
}

int cosineSimilarity (int user, int movie)
{
	// Declare variables for use in function
    int rating = 0;
    int minimumPairs = 1;
    float relevantMovies[1000][4]; // 0: item ID - 1: user ID - 2: user average - 3: item similarity weight
    int i, j, count;
    float numerator, denominator, denominatorLeft, denominatorRight;
    
    for (i=0; i<1000; i++)
    {
        relevantMovies[i][0] = 2000;    // Initialize user ID to an unobtainable value for validation usage
        relevantMovies[i][1] = 0;
        relevantMovies[i][2] = 0;
        relevantMovies[i][3] = 0;
    }
    
    // Find movies with co-rated pairs
    for (i=0; i<1000; i++)
    {
        count = 0;
        for (j=0; j<200; j++)
        {
            if ((ratings[j][i] != 0 && ratings[j][i] != 9) && (ratings[j][i] != 0 && ratings[j][i] != 9) &&
                (ratings[j][movie] != 0 && ratings[j][movie] != 9) && (ratings[j][movie] != 0 && ratings[j][movie] != 9))
            {
                count++;
            }
        }
        if (count >= minimumPairs)
        {
            count = 0;
            for (j=0; j<200; j++)
            {
                if ((ratings[j][i] != 0 && ratings[j][i] != 9) && (ratings[j][i] != 0 && ratings[j][i] != 9) &&
                    (ratings[j][movie] != 0 && ratings[j][movie] != 9) && (ratings[j][movie] != 0 && ratings[j][movie] != 9))
                {
                    relevantMovies[count][0] = i;
                    relevantMovies[count][1] = j;
                    count++;
                }
            }
        }
    }
    
    // Calculate user averages for stored users
    for (i=0; i<1000; i++)
    {
        if (relevantMovies[i][0] != 2000)
        {
            count = 0;
            for (j=0; j<1000; j++)
            {
                if ((ratings[(int)relevantMovies[i][1]][j] != 0) && (ratings[(int)relevantMovies[i][1]][j] != 9))
                {
                    relevantMovies[i][2] += ratings[(int)relevantMovies[i][1]][j];
                    count++;
                }
            }
            relevantMovies[i][2] = relevantMovies[i][2] / count;
        }
    }
    
    // Calculate similarity weights for stored movies
    i = 0;
    numerator = 0;
    denominator = 0;
    denominatorLeft = 0;
    denominatorRight = 0;
    while ((i < 1000) && (relevantMovies[i][0] != 2000))
    {
        numerator += (ratings[(int)relevantMovies[i][1]][movie] - relevantMovies[i][2]) * (ratings[(int)relevantMovies[i][1]][(int)relevantMovies[i][0]] - relevantMovies[i][2]);
        denominatorLeft += (ratings[(int)relevantMovies[i][1]][movie] - relevantMovies[i][2]) * (ratings[(int)relevantMovies[i][1]][movie] - relevantMovies[i][2]);
        denominatorRight += (ratings[(int)relevantMovies[i][1]][(int)relevantMovies[i][0]] - relevantMovies[i][2]) * (ratings[(int)relevantMovies[i][1]][(int)relevantMovies[i][0]] - relevantMovies[i][2]);
        j = i + 1;
        while ((j < 1000) && (relevantMovies[j][0] == relevantMovies[i][0]))
        {
            numerator += (ratings[(int)relevantMovies[j][1]][movie] - relevantMovies[j][2]) * (ratings[(int)relevantMovies[j][1]][(int)relevantMovies[j][0]] - relevantMovies[j][2]);
            denominatorLeft += (ratings[(int)relevantMovies[j][1]][movie] - relevantMovies[j][2]) * (ratings[(int)relevantMovies[j][1]][movie] - relevantMovies[j][2]);
            denominatorRight += (ratings[(int)relevantMovies[j][1]][(int)relevantMovies[j][0]] - relevantMovies[j][2]) * (ratings[(int)relevantMovies[j][1]][(int)relevantMovies[j][0]] - relevantMovies[j][2]);
            j++;
        }
        denominator = sqrt (denominatorLeft) * sqrt (denominatorRight);
        relevantMovies[i][3] = numerator / denominator;
        i = j;
    }
    
    // Calculate rating
    numerator = 0;
    denominator = 0;
    i = 0;
    while ((i < 1000) && (relevantMovies[i][0] != 2000))
    {
        numerator += relevantMovies[i][3] * ratings[user][(int)relevantMovies[i][0]];
        denominator += fabsf (relevantMovies[i][3]);
        j = i + 1;
        while ((j < 1000) && (relevantMovies[j][0] == relevantMovies[i][0]))
        {
            j++;
        }
        i = j;
    }
    rating = fabsf (numerator / denominator) + 0.5;
    
    if (rating > 5)
	{
		rating = 5;
	}
	if (rating < 1)
	{
		rating = 1;
	}
    
	return rating;
}

int custom (int user, int movie)
{
	int rating = 0;
    
    int pearsonRating = pearsonStandard (user, movie);
    int pearsonCaseRating = pearsonCase (user, movie);
    int pearsonInverseRating = pearsonInverse (user, movie);
    int vectorRating = vector (user, movie);
    int cosineRating = cosineSimilarity (user, movie);
    
    rating = ((pearsonRating + pearsonCaseRating + pearsonInverseRating + vectorRating + cosineRating) + 0.5) / 5;

	return rating;
}

int main (void)
{
	// Declare variables for use in functions
	int i, j;
	FILE *infile;

	// Set all test data array values to out-of-range value
	for (i=0; i<500; i++)
	{
		for (j=0; j<1000; j++)
		{
			// Test values to be between 0 and 5, if outside, value is "null"
			ratings[i][j] = 9;
		}
	}
	for (i=0; i<100; i++)
	{
		for (j=0; j<1000; j++)
		{
			ratingResults[i][j] = 0;
		}
	}

	// Input rating array
	infile = fopen ("train.txt", "r");
	if (infile == NULL)
	{
		fprintf (stderr, "Can't open input file train.txt.\n");
		exit (1);
	}
	else
	{
		int ratingInput;
		i = 0;
		j = 0;
		while (fscanf (infile, "%d", &ratingInput) != EOF)
		{
			if (j < 1000)
			{
				ratings[i][j] = ratingInput;
				j++;
			}
			if (j >= 1000)
			{
				j = 0;
				i++;
			}
		}
	}
	fclose (infile);

	// Ask user which file to use
	int testFile;
	char testFilename[20];
	char resultFilename[20];
	printf ("\nWhich file would you like to import for testing?\n");
	printf ("1 - test5.txt\t");
	printf ("2 - test10.txt\t");
	printf ("3 - test20.txt\n");
	scanf ("%d", &testFile);

	// Calculate user offset (200, 300, or 400) for printout of end results
	if (testFile == 1)
	{
		userOffset = 200;
		testNumber = 5;
		strcpy (testFilename, "test5.txt");
		strcpy (resultFilename, "result5.txt");
	}
	if (testFile == 2)
	{
		userOffset = 300;
		testNumber = 10;
		strcpy (testFilename, "test10.txt");
		strcpy (resultFilename, "result10.txt");
	}
	if (testFile == 3)
	{
		userOffset = 400;
		testNumber = 20;
		strcpy (testFilename, "test20.txt");
		strcpy (resultFilename, "result20.txt");
	}
	// Input file to operate on
	infile = fopen (testFilename, "r");
	if (infile == NULL)
	{
		fprintf (stderr, "Can't open input file for testing.\n");
		exit (1);
	}
	else
	{
		int ratingInput;
		i = 0;
		j = 0;
		while (fscanf (infile, "%d%d%d", &i, &j, &ratingInput) != EOF)
		{
			ratings[i-1][j-1] = ratingInput;
		}
	}
	fclose (infile);

	// Call function to trigger rating calculation
	calculate ();

	// Output results
	int user, movie, rating;
	FILE *outfile;
	outfile = fopen (resultFilename, "w");
	if (outfile == NULL)
	{
		fprintf (stderr, "Can't open output file for writing.\n");
		exit (1);
	}
	else
	{
		for (i=0; i<100; i++)
		{
			for (j=0; j<1000; j++)
			{
				if (ratingResults[i][j] != 0)
				{
					user = i + userOffset + 1;
					movie = j + 1;
					rating = ratingResults[i][j];
					fprintf (outfile, "%d %d %d\n", user, movie, rating);
				}
			}
		}
	}
	fclose (outfile);
	printf ("Results successfully outputted.\n");
}

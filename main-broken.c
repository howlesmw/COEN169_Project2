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
int ratings[500][1000];             // Holds all training data and test data by user ID and movie ID
int ratingResults[100][1000];       // Holds all calculated results by offset-adjusted user ID and movie ID
int testNumber;                     // Tells the program which set of data we're looking at (5, 10, or 20)
int userOffset;                     // Offset for input and output adjustment (200, 300, or 400)

// Function prototypes
int pearsonStandard (int, int);     // Pearson Correlation
int pearsonInverse (int, int);      // Inverse User Frequency using Pearson Correlation
int pearsonCase (int, int);         // Case Modification using Pearson Correlation
int vector (int, int);              // Vector Similarity
int cosineSimilarity (int, int);    // Adjusted Cosine Similarity
int custom (int, int);              // My custom algorithm

/* Serves as a switchboard to the chosen rating prediction algorithm */
void calculate (void)
{
	/* Declare variables for use in functions */
    // Counters
	int i, j;
    // Variable to hold calculated rating
    int rating;
    // Variable to hold which algorithm the user chooses for use in the switch loop
    int algorithm;

	/* Ask user which algorithm to use */
	printf ("\nWhich algorithm would you like to use to predict ratings?\n");
	printf ("1 - Pearson Correlation\n");
	printf ("2 - Vector Similarity\n");
	printf ("3 - Pearson Correlation - Inverse User Frequency\n");
	printf ("4 - Pearson Correlation - Case Modification\n");
	printf ("5 - Cosine Similarity\n");
	printf ("6 - Custom Algorithm\n");
    // Scan user answer
	scanf ("%d", &algorithm);

	/* Iterate through array to find ratings that need to be calculated */
    // Start user counter at userOffset (starting user ID - 1)
	for (i=userOffset; i<userOffset+100; i++)
	{
        // Start movie counter at 0 (movie ID - 1)
		for (j=0; j<1000; j++)
		{
            // If the value stored for that user and ID is 0 (meaning a non-null 0, "null" being defined as 9)
			if (ratings[i][j] == 0)
			{
                // Extract the user ID and movie ID for use in the selected algorithm
				int user = i;
				int movie = j;
				/* Call the appropriate calculation function */
				switch (algorithm)
				{
                        // Input 1 means Pearson Correlation
					case 1:
						rating = pearsonStandard (user, movie);
						break;
						
                        // Input 2 means Vector Similarity
					case 2:
						rating = vector (user, movie);
						break;
						
                        // Input 3 means Inverse User Frequency using Pearson Correlation
					case 3:
						rating = pearsonInverse (user, movie);
						break;
						
                        // Input 4 means Case Modification using Pearson Correlation
					case 4:
						rating = pearsonCase (user, movie);
						break;
						
                        // Input 5 means Adjusted Cosine Similarity
					case 5:
						rating = cosineSimilarity (user, movie);
						break;
						
                        // Input 6 means my custom algorithm
					case 6:
						rating = custom (user, movie);
						break;
					
                        // If value is not within defined range, default to case 1
					default:
						rating = pearsonStandard (user, movie);
						break;
				}
                // Conver user ID to user index and set that corresponding user/movie value to the calculated rating
				ratingResults[user-userOffset][movie] = rating;
			}
		}
	}
}

/* Pearson Correlation */
int pearsonStandard (int user, int movie)
{
	/* Declare and initialize variables for use in function */
    // Variables to hold calculated rating
	int rating = 0;
    float ratingFloat;
    // Array to hold the movies that we're using as the sample population, size is the test size (5, 10, or 20)
	int testMovies[testNumber];
    // Array to hold users with valid ratings on all of the movies in the sample population
    // Set to hold 200 users, though it's highly unlikely all 200 users will be relevant to the calculation
    // 0: average --- 1: std dev --- 2: similarity weight --- 3: user ID
	float relevantUsers[200][4];
    // Variable for average rating of our target user
	float userAverage = 0;
    // Variable for standard deviation of our target user
	float userStdDev = 0;
    // Variables to hold numerator and denominators of large formulas for simpler calculation
    float numerator, denominator;
    // Counters
	int i, j, count, relevantCount;
	
    // Initialize all elements of relevant users array to 0; allows us to test to make sure entry is valid
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
		{
			relevantUsers[i][j] = 0;
		}
	}

	/* Calculate average rating for user in question */
	// Zero out counter flag
    count = 0;
    // Test to see which movies are going to be in the sample population
	for (i=0; i<1000; i++)
	{
        // If the user contains a valid rating for movie located at index i
        // 0 indicates a rating needing to be calculated
        // 9 indicates a "null" value, meaning that the test data did not contain any information for that user/movie
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9))
		{
            // Sequentially store the movie IDs relevant to the calculation
			testMovies[count] = i;
            // Add the rating at that index to the user average
			userAverage += ratings[user][i];
            // Increment the flag to indicate another relevant movie
			count++;
		}
	}
    // Calculate average by dividing the sum of ratings for relevant movies by the number of relevant movies
	userAverage = userAverage / (float)count;

    /* Calculate average ratings for relevant users in training population */
    // Zero out counter flag for number of relevant users
	relevantCount = 0;
    // Traverse array of users
	for (i=0; i<200; i++)
	{
        // Zero out counter flag
		count = 0;
        // Test to see if user has ratings for relevant movies
		for (j=0; j<testNumber; j++)
		{
            // If the user has a non-zero and non-null rating for that movie
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
			{
                // Increment the counter flag
				count++;
			}
		}
        // If the user has valid ratings for all relevant movies
		if (count == testNumber)
		{
            // Traverse the array of their stored ratings
			for (j=0; j<testNumber; j++)
			{
                // Verify that the user has a non-zero and non-null rating for that movie
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
                    // Store that user ID
					relevantUsers[relevantCount][3] = i;
                    // Add the corresponding rating to the user average
					relevantUsers[relevantCount][0] += ratings[i][testMovies[j]];
				}
			}
            // Calculate average by dividing the sum of ratings by the number of movies
			relevantUsers[relevantCount][0] = relevantUsers[relevantCount][0] / count;
            // Increment the relevant users counter flag
			relevantCount++;
		}
	}

	/* Compute denominator elements of similarity weight */
    // Compute standard deviation for target user
	for (i=0; i<testNumber; i++)
	{
        // Add subsection of formula to create summation
		userStdDev += (ratings[user][testMovies[i]] - userAverage) * (ratings[user][testMovies[i]] - userAverage);
	}
    
    // Take the square root of that summation
	userStdDev = sqrt (userStdDev);
    // Compute standard deviation for all relevant users
	for (i=0; i<200; i++)
	{
        // Verify that there is a valid user ID at that index
		if (relevantUsers[i][3] != 0)
		{
            // Compute standard deviation for selected user
			for (j=0; j<testNumber; j++)
			{
                // Add subsection of formula to create summation
                relevantUsers[i][1] += (ratings[i][testMovies[j]] - relevantUsers[i][0]) * (ratings[i][testMovies[j]] - relevantUsers[i][0]);
			}
            // Take the square root of that summation
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}

	/* Compute similarity weight */
    // Zero out numerator and denominator
	numerator = 0;
	denominator = 0;
    
    // If there are no relevant users to assist in computing rating
	if (relevantCount == 0)
	{
        // Use the +0.5 trick to "round" the float to the nearest integer to improve accuracy as opposed to truncation
        rating = userAverage + 0.5;
        // Return the calculated rating to the calculation function
		return rating;
	}

    // If there are relevant users, compute their similarity weights
	for (i=0; i<200; i++)
	{
        // Verify that there is a valid user ID at that index
		if (relevantUsers[i][3] != 0)
		{
            // Compute similarity weight for selected user
			for (j=0; j<testNumber; j++)
			{
                // Create summation of numerator of similarity weight equation
				numerator += (ratings[user][testMovies[j]] - userAverage) * (ratings[(int)relevantUsers[i][3]][testMovies[j]] - relevantUsers[i][0]);
			}
            // Create denominator of similarity weight equation
			denominator = userStdDev * relevantUsers[i][1];
            // Calculate simiarlity weight
			relevantUsers[i][2] = numerator / denominator;
		}
	}

	/* Compute prediction */
    // Zero out numerator and denominator for reuse
	numerator = 0;
	denominator = 0;
    // Iterate through relevant users array
	for (i=0; i<200; i++)
	{
        // Verify that there is valid user ID at that index
		if (relevantUsers[i][3] != 0)
		{
            // Iterate for all relevant movies
			for (j=0; j<testNumber; j++)
			{
                // Calculate numerator and denominator for the provided formula for Pearson Correlation
				numerator += relevantUsers[i][2] * (ratings[i][testMovies[j]] - relevantUsers[i][0]);
				denominator += fabsf(relevantUsers[i][2]);
                printf ("Prediction Numerator: %3.3f\t\tDenominator: %3.3f\n", numerator, denominator);
			}
		}
	}
    // Calculate the final formula for Pearson Correlation
	ratingFloat = (numerator + userAverage) / denominator;
    printf ("Rating float: %3.3f\n", ratingFloat);
    
    // If computed rating has a divide-by-zero error
	if ((ratingFloat < 0) || (ratingFloat > 5))
	{
        // Use the +0.5 trick to "round" the float to the nearest integer to improve accuracy as opposed to truncation
        rating = userAverage + 0.5;
	}
	
    // Round any positive rating to the nearest integer
    if (ratingFloat > 0)
    {
        // Use the +0.5 trick to "round" the float to the nearest integer to improve accuracy as opposed to truncation
        rating = ratingFloat + 0.5;
    }
    
    // If the calculated rating is above the upper bound, set it to the upper bound
	if (rating > 5)
	{
		rating = 5;
	}

    // If the calculated rating is below the lower bound
    if (ratingFloat < 1)
    {
        // Set the rating to the lower bound
        rating = 1;
    }
	
    printf ("Rating: %d\n", rating);
    
	// Return final calculated value
	return rating;	
}

/* Inverse User Frequency using Pearson Correlation */
int pearsonInverse (int user, int movie)
{
	/* Declare and initialize variables for use in function */
    // Variables to hold calculated rating
	int rating = 0;
    float ratingFloat;
    // Array to hold the movies that we're using as the sample population, size is the test size (5, 10, or 20)
    // 0: movie ID --- 1: Inverse User Frequency for that movie
	float testMovies[testNumber][2];
    // Array to hold users with valid ratings on all of the movies in the sample population
    // Set to hold 200 users, though it's highly unlikely all 200 users will be relevant to the calculation
    // 0: average --- 1: std dev --- 2: similarity weight --- 3: user ID
	float relevantUsers[200][5];
    // Variables for calculating IUF
    int usersRated;
    int totalUsers = 200;
    // Variable for average rating of our target user
	float userAverage = 0;
    // Variable for standard deviation of our target user
	float userStdDev = 0;
    // Variables to hold numerator and denominators of large formulas for simpler calculation
    float numerator, denominator;
    // Counters
	int i, j, count, relevantCount;
	
    // Initialize all elements of relevant users array to 0; allows us to test to make sure entry is valid
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
		{
			relevantUsers[i][j] = 0;
		}
	}
    
    /* Find the relevant movies */
    // Zero out counter flat
    count = 0;
	for (i=0; i<1000; i++)
	{
        // If the user contains a valid rating for movie located at index i
        // 0 indicates a rating needing to be calculated
        // 9 indicates a "null" value, meaning that the test data did not contain any information for that user/movie
        if ((ratings[user][i] != 0) && (ratings[user][i] != 9))
        {
            // Sequentially store the movie IDs relevant to the calculation
            testMovies[count][0] = i;
            count++;
        }
    }
    
    /* Calculate IUF for each relevant movie */
    for (i=0; i<testNumber; i++)
    {
        // Zero out the number of users that have rated the movie counted in each iteration
        usersRated = 0;
        for (j=0; j<200; j++)
        {
            // If the user contains a valid rating for movie located at index i
            // 0 indicates a rating needing to be calculated
            // 9 indicates a "null" value, meaning that the test data did not contain any information for that user/movie
            if ((ratings[j][(int)testMovies[i][0]] != 0) && (ratings[j][(int)testMovies[i][0]] != 9))
            {
                usersRated++;
            }
        }
        testMovies[i][1] = log ((float)totalUsers / (float)usersRated);
    }
    
	/* Calculate average rating for user in question */
	// Zero out counter flag
    count = 0;
	for (i=0; i<testNumber; i++)
	{
        // If the user contains a valid rating for movie located at index i
        // 0 indicates a rating needing to be calculated
        // 9 indicates a "null" value, meaning that the test data did not contain any information for that user/movie
		if ((ratings[user][(int)testMovies[i][0]] != 0) && (ratings[user][(int)testMovies[i][0]] != 9))
		{
            // Add the rating at that index to the user average
			userAverage += ratings[user][(int)testMovies[i][0]];
			count++;
		}
	}
    // Calculate average
	userAverage = userAverage / (float)count;
    
    /* Calculate average ratings for relevant users in training population */
    // Zero out counter flag for number of relevant users
	relevantCount = 0;
    // Traverse array of users
	for (i=0; i<200; i++)
	{
        // Zero out counter flag
		count = 0;
        // Test to see if user has ratings for relevant movies
		for (j=0; j<testNumber; j++)
		{
            // If the user has a non-zero and non-null rating for that movie
			if ((ratings[i][(int)testMovies[j][0]] != 0) && (ratings[i][(int)testMovies[j][0]] != 9))
			{
                // Increment the counter flag
				count++;
			}
		}
        // If the user has valid ratings for all relevant movies
		if (count == testNumber)
		{
            // Traverse the array of their stored ratings
			for (j=0; j<testNumber; j++)
			{
                // Verify that the user has a non-zero and non-null rating for that movie
				if ((ratings[i][(int)testMovies[j][0]] != 0) && (ratings[i][(int)testMovies[j][0]] != 9))
				{
                    // Store that user ID
					relevantUsers[relevantCount][3] = i;
                    // Add the corresponding rating to the user average
					relevantUsers[relevantCount][0] += ratings[i][j] * testMovies[j][1];
				}
			}
            // Calculate average by dividing the sum of ratings by the number of movies
			relevantUsers[relevantCount][0] = relevantUsers[relevantCount][0] / count;
            // Increment the relevant users counter flag
			relevantCount++;
		}
	}
    
	/* Compute denominator elements of similarity weight */
    // Compute standard deviation for target user
	for (i=0; i<testNumber; i++)
	{
        // Add subsection of formula to create summation
		userStdDev += ((ratings[user][(int)testMovies[i][0]] * testMovies[i][1]) - userAverage) * ((ratings[user][(int)testMovies[i][0]] * testMovies[i][1]) - userAverage);
	}
    // Take the square root of that summation
	userStdDev = sqrt (userStdDev);
    // Compute standard deviation for all relevant users
	for (i=0; i<200; i++)
	{
        // Verify that there is a valid user ID at that index
		if (relevantUsers[i][3] != 0)
		{
            // Compute standard deviation for selected user
			for (j=0; j<testNumber; j++)
			{
                // Add subsection of formula to create summation
                relevantUsers[i][1] += ((ratings[i][(int)testMovies[j][0]] * testMovies[j][1]) - relevantUsers[i][0]) * ((ratings[i][(int)testMovies[j][0]] * testMovies[j][1]) - relevantUsers[i][0]);
			}
            // Take the square root of that summation
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}
    
	/* Compute similarity weight */
    // Zero out numerator and denominator
	numerator = 0;
	denominator = 0;
    // If there are no relevant users to assist in computing rating
	if (relevantCount == 0)
	{
        // Use the +0.5 trick to "round" the float to the nearest integer to improve accuracy as opposed to truncation
        rating = userAverage + 0.5;
        // Return the calculated rating to the calculation function
		return rating;
	}
    // If there are relevant users, compute their similarity weights
	for (i=0; i<200; i++)
	{
        // Verify that there is a valid user ID at that index
		if (relevantUsers[i][3] != 0)
		{
            // Compute similarity weight for selected user
			for (j=0; j<testNumber; j++)
			{
                // Create summation of numerator of similarity weight equation
				numerator += ((ratings[user][(int)testMovies[j][0]] * testMovies[j][1]) - userAverage) * ((ratings[i][(int)testMovies[j][0]] * testMovies[j][1]) - relevantUsers[i][0]);
			}
            // Create denominator of similarity weight equation
			denominator = userStdDev * relevantUsers[i][1];
            // Calculate simiarlity weight
			relevantUsers[i][2] = numerator / denominator;
		}
	}
    
	/* Compute prediction */
    // Zero out numerator and denominator for reuse
	numerator = 0;
	denominator = 0;
    // Iterate through relevant users array
	for (i=0; i<200; i++)
	{
        // Verify that there is valid user ID at that index
		if (relevantUsers[i][3] != 0)
		{
            // Iterate for all relevant movies
			for (j=0; j<testNumber; j++)
			{
                // Calculate numerator and denominator for the provided formula for Pearson Correlation
				numerator += relevantUsers[i][2] * ((ratings[i][(int)testMovies[j][0]] * testMovies[j][1]) - relevantUsers[i][0]);
				denominator += fabsf(relevantUsers[i][2]);
			}
		}
	}
    // Calculate the final formula for Pearson Correlation
	ratingFloat = (numerator + userAverage) / denominator;
	
    // Round any positive rating to the nearest integer
    if (ratingFloat > 0)
    {
        // Use the +0.5 trick to "round" the float to the nearest integer to improve accuracy as opposed to truncation
        rating = ratingFloat + 0.5;
    }
    
    // If the calculated rating is above the upper bound, set it to the upper bound
	if (rating > 5)
	{
		rating = 5;
	}
    
    // If the calculated rating is below the lower bound
    if (ratingFloat < 1)
    {
        // Set the rating to the lower bound
        rating = 1;
    }
	
	// Return final calculated value
	return rating;
}

/* Case Modification using Pearson Correlation */
int pearsonCase (int user, int movie)
{
    /* Declare and initialize variables for use in function */
    // Variables to hold calculated rating
	int rating = 0;
    float ratingFloat;
    // Array to hold the movies that we're using as the sample population, size is the test size (5, 10, or 20)
	int testMovies[testNumber];
    // Array to hold users with valid ratings on all of the movies in the sample population
    // Set to hold 200 users, though it's highly unlikely all 200 users will be relevant to the calculation
    // 0: average --- 1: std dev --- 2: similarity weight --- 3: user ID
	float relevantUsers[200][4];
    // Variable for average rating of our target user
	float userAverage = 0;
    // Variable for standard deviation of our target user
	float userStdDev = 0;
    // Variables to hold numerator and denominators of large formulas for simpler calculation
    float numerator, denominator;
    // Counters
	int i, j, count, relevantCount;
    // Variable for case amplification power
    float caseAmp = 2.5;
	
    // Initialize all elements of relevant users array to 0; allows us to test to make sure entry is valid
	for (i=0; i<200; i++)
	{
		for (j=0; j<4; j++)
		{
			relevantUsers[i][j] = 0;
		}
	}
    
	/* Calculate average rating for user in question */
	// Zero out counter flag
    count = 0;
    // Test to see which movies are going to be in the sample population
	for (i=0; i<1000; i++)
	{
        // If the user contains a valid rating for movie located at index i
        // 0 indicates a rating needing to be calculated
        // 9 indicates a "null" value, meaning that the test data did not contain any information for that user/movie
		if ((ratings[user][i] != 0) && (ratings[user][i] != 9))
		{
            // Sequentially store the movie IDs relevant to the calculation
			testMovies[count] = i;
            // Add the rating at that index to the user average
			userAverage += ratings[user][i];
            // Increment the flag to indicate another relevant movie
			count++;
		}
	}
    // Calculate average by dividing the sum of ratings for relevant movies by the number of relevant movies
	userAverage = userAverage / (float)count;
    
    /* Calculate average ratings for relevant users in training population */
    // Zero out counter flag for number of relevant users
	relevantCount = 0;
    // Traverse array of users
	for (i=0; i<200; i++)
	{
        // Zero out counter flag
		count = 0;
        // Test to see if user has ratings for relevant movies
		for (j=0; j<testNumber; j++)
		{
            // If the user has a non-zero and non-null rating for that movie
			if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
			{
                // Increment the counter flag
				count++;
			}
		}
        // If the user has valid ratings for all relevant movies
		if (count == testNumber)
		{
            // Traverse the array of their stored ratings
			for (j=0; j<testNumber; j++)
			{
                // Verify that the user has a non-zero and non-null rating for that movie
				if ((ratings[i][testMovies[j]] != 0) && (ratings[i][testMovies[j]] != 9))
				{
                    // Store that user ID
					relevantUsers[relevantCount][3] = i;
                    // Add the corresponding rating to the user average
					relevantUsers[relevantCount][0] += ratings[i][j];
				}
			}
            // Calculate average by dividing the sum of ratings by the number of movies
			relevantUsers[relevantCount][0] = relevantUsers[relevantCount][0] / count;
            // Increment the relevant users counter flag
			relevantCount++;
		}
	}
    
	/* Compute denominator elements of similarity weight */
    // Compute standard deviation for target user
	for (i=0; i<testNumber; i++)
	{
        // Add subsection of formula to create summation
		userStdDev += (ratings[user][testMovies[i]] - userAverage) * (ratings[user][testMovies[i]] - userAverage);
	}
    // Take the square root of that summation
	userStdDev = sqrt (userStdDev);
    // Compute standard deviation for all relevant users
	for (i=0; i<200; i++)
	{
        // Verify that there is a valid user ID at that index
		if (relevantUsers[i][3] != 0)
		{
            // Compute standard deviation for selected user
			for (j=0; j<testNumber; j++)
			{
                // Add subsection of formula to create summation
                relevantUsers[i][1] += (ratings[i][testMovies[j]] - relevantUsers[i][0]) * (ratings[i][testMovies[j]] - relevantUsers[i][0]);
			}
            // Take the square root of that summation
			relevantUsers[i][1] = sqrt (relevantUsers[i][1]);
		}
	}
    
	/* Compute similarity weight */
    // Zero out numerator and denominator
	numerator = 0;
	denominator = 0;
    // If there are no relevant users to assist in computing rating
	if (relevantCount == 0)
	{
        // Use the +0.5 trick to "round" the float to the nearest integer to improve accuracy as opposed to truncation
        rating = userAverage + 0.5;
        // Return the calculated rating to the calculation function
		return rating;
	}
    // If there are relevant users, compute their similarity weights
	for (i=0; i<200; i++)
	{
        // Verify that there is a valid user ID at that index
		if (relevantUsers[i][3] != 0)
		{
            // Compute similarity weight for selected user
			for (j=0; j<testNumber; j++)
			{
                // Create summation of numerator of similarity weight equation
				numerator += (ratings[user][testMovies[j]] - userAverage) * (ratings[i][testMovies[j]] - relevantUsers[i][0]);
			}
            // Create denominator of similarity weight equation
			denominator = userStdDev * relevantUsers[i][1];
            // Calculate simiarlity weight before case amplification
			relevantUsers[i][2] = numerator / denominator;
            
            // Calculate similarity weight with case amplification
            relevantUsers[i][2] = relevantUsers[i][2] * abs(pow (relevantUsers[i][2], caseAmp));
		}
	}
    
	/* Compute prediction */
    // Zero out numerator and denominator for reuse
	numerator = 0;
	denominator = 0;
    // Iterate through relevant users array
	for (i=0; i<200; i++)
	{
        // Verify that there is valid user ID at that index
		if (relevantUsers[i][3] != 0)
		{
            // Iterate for all relevant movies
			for (j=0; j<testNumber; j++)
			{
                // Calculate numerator and denominator for the provided formula for Pearson Correlation
				numerator += relevantUsers[i][2] * (ratings[i][testMovies[j]] - relevantUsers[i][0]);
				denominator += fabsf(relevantUsers[i][2]);
			}
		}
	}
    // Calculate the final formula for Pearson Correlation
	ratingFloat = (numerator + userAverage) / denominator;
	
    // Round any positive rating to the nearest integer
    if (ratingFloat > 0)
    {
        // Use the +0.5 trick to "round" the float to the nearest integer to improve accuracy as opposed to truncation
        rating = ratingFloat + 0.5;
    }
    
    // If the calculated rating is above the upper bound, set it to the upper bound
	if (rating > 5)
	{
		rating = 5;
	}
    
    // If the calculated rating is below the lower bound
    if (ratingFloat < 1)
    {
        // Set the rating to the lower bound
        rating = 1;
    }
    
	// Return final calculated value
	return rating;
}

/* Vector Similarity */
int vector (int user, int movie)
{
	int rating = 0;

	return rating;
}

/* Adjusted Cosine Similarity */
int cosineSimilarity (int user, int movie)
{
	int rating = 0;
    
	return rating;
}

/* My custom algorithm */
int custom (int user, int movie)
{
    // Declare and initialize variable to hold calculated rating
	int rating = 0;
    
    // Calculate sub-ratings using the other five algorithms
    int pearsonStandardRating = pearsonStandard (user, movie);
    int pearsonCaseRating = pearsonCase (user, movie);
    int pearsonInverseRating = pearsonInverse (user, movie);
    int vectorRating = vector (user, movie);
    int cosineSimilarityRating = cosineSimilarity (user, movie);
    
    // Average the sub-ratings together because I'm lazy and don't feel like writing more code
    rating = (pearsonStandardRating + pearsonCaseRating + pearsonInverseRating + vectorRating + cosineSimilarityRating) / 5;

    // Return final calculated value
	return rating;
}

int main (void)
{
	/* Declare variables for use in functions */
    // Counters
	int i, j;
    // File pointer
	FILE *infile, *outfile;
    // Variables for inputting test data to array
    int testFile;
	char testFilename[20];
    // Variables for outputting results to file
    int user, movie, rating;
    char resultFilename[20];

	/* Set all test data array values to out-of-range value */
    // Test values are to be between 0 and 5; by initializing array to 9, we know what the "garbage" values are
	for (i=0; i<500; i++)
	{
		for (j=0; j<1000; j++)
		{
			ratings[i][j] = 9;
		}
	}
    // There won't be any 0s in the results array that we put there so 0 is a good "garbage" value
	for (i=0; i<100; i++)
	{
		for (j=0; j<1000; j++)
		{
			ratingResults[i][j] = 0;
		}
	}

	/* Input training file data into array */
    // Open file
	infile = fopen ("train.txt", "r");
    // Check to make sure the file opened correctly
	if (infile == NULL)
    {
		// If the file didn't open correctly, alert the user and quit the program
        fprintf (stderr, "Can't open input file train.txt.\n");
		exit (1);
	}
    // If the file did open correctly, alert the user and continue the program
	else
	{
        // Input the data from the file into the array
		int ratingInput;
        // Start counting at 0; index will be user/movie number - 1 (i.e. U:25 M:30 = i:24 j:29)
		i = 0;
		j = 0;
        // Read file contents sequentially
		while (fscanf (infile, "%d", &ratingInput) != EOF)
		{
            // If we haven't read all the movies for that user yet
			if (j < 1000)
			{
                // Input the data into the corresponding index
				ratings[i][j] = ratingInput;
                // Increment the movie counter
				j++;
			}
            // If we have read all the movies for that user
			if (j >= 1000)
			{
                // Reset movie counter to index 0
				j = 0;
                // Increment user counter
				i++;
			}
		}
	}
    // Close the file because we're done with it
	fclose (infile);

	/* Ask user which file to use */
	printf ("\nWhich file would you like to import for testing?\n");
	printf ("1 - test5.txt\t");
	printf ("2 - test10.txt\t");
	printf ("3 - test20.txt\n");
    // Scan user answer
	scanf ("%d", &testFile);

	/* Calculate user offset (200, 300, or 400) for printout of end results */
    switch (testFile)
    {
            // Input 1 means test5.txt, users 201-300
        case 1:
            userOffset = 200;
            testNumber = 5;
            strcpy (testFilename, "test5.txt");
            strcpy (resultFilename, "result5.txt");
            break;
            
            // Input 2 means test10.txt, users 301-400
        case 2:
            userOffset = 300;
            testNumber = 10;
            strcpy (testFilename, "test10.txt");
            strcpy (resultFilename, "result10.txt");
            break;
            
            // Input 3 means test20.txt, users 401-500
        case 3:
            userOffset = 400;
            testNumber = 20;
            strcpy (testFilename, "test20.txt");
            strcpy (resultFilename, "result20.txt");
            break;
            
            // If value is not within predefined range, default to case 1
        default:
            userOffset = 200;
            testNumber = 5;
            strcpy (testFilename, "test5.txt");
            strcpy (resultFilename, "result5.txt");
            break;
    }

	/* Input file to operate on */
    // Open file
	infile = fopen (testFilename, "r");
    // Check to make sure file opens correctly
	if (infile == NULL)
	{
        // If the file didn't open correctly, alert the user and quit the program
		fprintf (stderr, "Can't open input file for testing.\n");
		exit (1);
	}
    // If the file did open correctly, alert the user and continue the program
	else
	{
        // Input the data from the file into the array
		int ratingInput;
        // Start counting at 0; index will be user/movie number - 1 (i.e. U:25 M:30 = i:24 j:29)
		i = 0;
		j = 0;
        // Read file contents sequentially
		while (fscanf (infile, "%d%d%d", &i, &j, &ratingInput) != EOF)
		{
            // Input U: M: R: format data blocks (remember that i = U - 1 and j = M - 1)
			ratings[i-1][j-1] = ratingInput;
		}
	}
    // Close the file because we're done with it
	fclose (infile);

	/* Call function to trigger rating calculation */
	calculate ();

	/* Output results */
    // Open file
	outfile = fopen (resultFilename, "w");
    // Check to make sure file opens correctly
	if (outfile == NULL)
	{
        // If the file didn't open correctly, alert the user and quit the program
		fprintf (stderr, "Can't open output file for writing.\n");
		exit (1);
	}
    // If the file did open correctly, alert the user and continue with the program
	else
	{
        // Store result data in the output file
		for (i=0; i<100; i++)
		{
			for (j=0; j<1000; j++)
			{
                // If the results array contains a valid calculated rating
				if (ratingResults[i][j] != 0)
				{
                    // Make sure to counteract our user/movie to array index conversion
					user = i + userOffset + 1;
					movie = j + 1;
                    // Extract the corresponding rating
					rating = ratingResults[i][j];
                    // Print the data into the output file
					fprintf (outfile, "%d %d %d\n", user, movie, rating);
				}
			}
		}
	}
    // Close the file because we're done with it
	fclose (outfile);
    // Alert the user that the file was succesfully modified.
	printf ("\nResults successfully outputted.\n");
}

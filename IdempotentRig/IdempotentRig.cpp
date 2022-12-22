//
//  IdempotentRig
//
//  Created by Greg Egan on 21/12/2022.
/*

What are the elements of an idempotent rig with two generators?

See:  https://mathstodon.xyz/@johncarlosbaez/109544916566242548

 A 'rig' R has a commutative associative addition, an associative multiplication that distributes over addition, an element 0 with r+0 = r and 0r = 0 = r0 for all r ∈ R, and an element 1 with 1r = r = r1 for all r ∈ R.

 A rig is 'idempotent' if rr = r for all r ∈ R.
 
 With two generators a, b, along with the identity 1, we have 7 monomials:
 
 1, a, b, ab, ba, aba, bab
 
Because (1+1)^2 = (1+1), we have 4 = 2, 5 = 3, 6 = 2, 7 = 3, ....

So all elements can be written as 7-tuples of integers from 0 to 3, which are the
coefficients for the 7 monomials.

We will sometimes work with 7-tuples of integers, and sometimes with single-integer
indices from 0 to 4^7-1.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

#define OUTPUT_FILE "IdempotentRig.txt"

#define NMONO 7
#define NINDEX (1<<(2*NMONO))
typedef uint16_t Index;

//	Multiplication table between monomials

int mtab[][NMONO] =
{
{0,1,2,3,4,5,6},
{1,1,3,3,5,5,3},
{2,4,2,6,4,4,6},
{3,5,3,3,5,5,3},
{4,4,6,6,4,4,6},
{5,5,3,3,5,5,3},
{6,4,6,6,4,4,6}
};

Index MTAB[NINDEX][NINDEX];
Index ATAB[NINDEX][NINDEX];

//	Text descriptions for the monomials

const char *mtext[] = {"1", "a", "b", "ab", "ba", "aba", "bab"};

//	Convert an integer 7-tuple to a single index

Index tupleToIndex(int *tuple)
{
Index res=0;
for (int i=0;i<NMONO;i++)
	{
	res |= tuple[i] << (2*i);
	};
return res;
}

//	Convert an index to an integer 7-tuple

void indexToTuple(Index index, int *tuple)
{
for (int i=0;i<NMONO;i++)
	{
	tuple[i] = (index >> (2*i)) & 0x3;
	};
}

//	Print a tuple as an expression

void printTuple(FILE *fp, int *tuple, bool par)
{
if (par) fprintf(fp,"(");
bool needPlus=false;
for (int k=0;k<NMONO;k++)
if (tuple[k]!=0)
	{
	if (needPlus) fprintf(fp,"+");
	if (k==0) fprintf(fp,"%d",tuple[k]);
	else
		{
		if (tuple[k]!=1) fprintf(fp,"%d",tuple[k]);
		fprintf(fp,"%s",mtext[k]);
		};
	needPlus=true;
	};
if (!needPlus) fprintf(fp,"0");
if (par) fprintf(fp,")");
}

//	Print an index number as an expression

void printIndex(FILE *fp, Index index, bool par)
{
int tuple[NMONO];
indexToTuple(index,tuple);
printTuple(fp,tuple,par);
}

//	Normalise an integer coefficient

int normCoeff(int c)
{
if (c>=4) return 2+(c%2); else return c;
}

//	Multiply two tuples

void multTuples(int *t1, int *t2, int *t12)
{
for (int i=0;i<NMONO;i++) t12[i]=0;

for (int i=0;i<NMONO;i++)
if (t1[i]!=0)
for (int j=0;j<NMONO;j++)
	t12[mtab[i][j]] += t1[i]*t2[j];
	
for (int i=0;i<NMONO;i++) t12[i]=normCoeff(t12[i]);
}

//	Multiply two indices

Index multIndices(Index i1, Index i2)
{
int t1[NMONO], t2[NMONO], t12[NMONO];
indexToTuple(i1,t1);
indexToTuple(i2,t2);
multTuples(t1,t2,t12);
return tupleToIndex(t12);
}

//	Add two tuples

void addTuples(int *t1, int *t2, int *t12)
{
for (int i=0;i<NMONO;i++) t12[i]=normCoeff(t1[i]+t2[i]);
}

//	Add two indices

Index addIndices(Index i1, Index i2)
{
int t1[NMONO], t2[NMONO], t12[NMONO];
indexToTuple(i1,t1);
indexToTuple(i2,t2);
addTuples(t1,t2,t12);
return tupleToIndex(t12);
}

//	Linked list node

struct node
{
int prev, next;
int count;
Index *elements;
};

//	Linked list of equivalence classes of elements

struct node LL[NINDEX];
int firstLL, countLL;

//	Equivalence class index for each element

Index eqc[NINDEX];

//	Sort equivalence classes by size

int ecmp(const void *a, const void *b)
{
int e1 = *((int *)(a));
int e2 = *((int *)(b));
int s1 = LL[e1].count;
int s2 = LL[e2].count;
return s1-s2;
}

//	Sort equivalence classes by first element

int ecmpFE(const void *a, const void *b)
{
int e1 = *((int *)(a));
int e2 = *((int *)(b));
int s1 = (int)(LL[e1].elements[0]);
int s2 = (int)(LL[e2].elements[0]);
return s1-s2;
}

//	Sort Index type

int cmpIndex(const void *a, const void *b)
{
int e1 = (int)(*((Index *)(a)));
int e2 = (int)(*((Index *)(b)));
return e1-e2;
}

//	Sort each equivalence class, and output both the minimum element of each class,
//	and all the classes.

void outputEC(FILE *fp)
{
int *cnum = new int[countLL];
int nc=0;
int ePtr = firstLL;
while (ePtr>=0)
	{
	qsort(LL[ePtr].elements,LL[ePtr].count,sizeof(LL[ePtr].elements[0]),cmpIndex);
	cnum[nc++] = ePtr;
	ePtr = LL[ePtr].next;
	};
qsort(cnum,nc,sizeof(cnum[0]),ecmpFE);

//	Output minimum element of each class

fprintf(fp,"{");
for (int i=0;i<countLL;i++)
	{
	ePtr = cnum[i];
	if (i!=0) fprintf(fp,",\n");
	printIndex(fp,LL[ePtr].elements[0],false);
	};
fprintf(fp,"}\n\n");

//	Output all elements of each class

fprintf(fp,"{");
for (int i=0;i<countLL;i++)
	{
	ePtr = cnum[i];
	if (i!=0) fprintf(fp,",\n");
	fprintf(fp,"{");
	for (int j=0;j<LL[ePtr].count;j++)
		{
		if (j!=0) fprintf(fp,", ");
		printIndex(fp,LL[ePtr].elements[j],false);
		};
	fprintf(fp,"}");
	};
fprintf(fp,"}\n");

delete [] cnum;
}

int main(int argc, const char * argv[])
{
//	Print the monomial multiplication table

printf("Monomial multiplication table\n     ");
for (int i=0;i<NMONO;i++) printf("%5s",mtext[i]);
printf("\n     ");
for (int i=0;i<NMONO;i++) printf("  ===");
printf("\n");
for (int i=0;i<NMONO;i++)
	{
	printf("%4s|",mtext[i]);
	for (int j=0;j<NMONO;j++) printf("%5s",mtext[mtab[i][j]]);
	printf("\n");
	};
printf("\n");

printf("Checking indexToTuple/tupleToIndex ...\n");
int tup[NMONO];
for (Index k=0;k<NINDEX;k++)
	{
	indexToTuple(k,tup);
	if (k!=tupleToIndex(tup))
		{
		printf("indexToTuple/tupleToIndex failure for index=%d\n",k);
		break;
		};
	};
printf("Done\n\n");

printf("First few sums ...\n");
for (Index k=0;k<20;k++)
	{
	printIndex(stdout,k,false);
	printf("\n");
	};
printf("\n\n");

//	Test multiplication

int aplusb[]={0,1,1,0,0,0,0};	//	a+b
int aplusb2[NMONO];
multTuples(aplusb,aplusb,aplusb2);
printf("Test multiplication\n");
printTuple(stdout,aplusb,true);
printf("^2 = ");
printTuple(stdout,aplusb2,false);
printf("\n\n");

//	Set up multiplication and addition tables

printf("Creating multiplication and addition tables ...\n");
for (Index x1=0;x1<NINDEX;x1++)
	{
	if (x1 % 100 == 0) printf("x1=%d / %d\n",x1,NINDEX);
	int t1[NMONO];
	indexToTuple(x1,t1);
	for (Index x2=0;x2<NINDEX;x2++)
		{
		int t2[NMONO], t12[NMONO];
		indexToTuple(x2,t2);
		
		multTuples(t1,t2,t12);
		MTAB[x1][x2] = tupleToIndex(t12);
		
		addTuples(t1,t2,t12);
		ATAB[x1][x2] = tupleToIndex(t12);
		};
	};
printf("Done\n\n");

//	Initialise the linked list of equivalent classes

firstLL=-1;
countLL=0;
for (int k=0;k<NINDEX;k++)
	{
	LL[k].prev = LL[k].next = -1;
	LL[k].count = 0;
	LL[k].elements = NULL;
	};
	
//	Put each of the NINDEX formal elements into an equivalence class based on its square
//
//	If x^2 = y^2, then x = x^2 = y^2 = y

for (Index x=0;x<NINDEX;x++)
	{
	Index sq = MTAB[x][x];
	eqc[x] = sq;
	if (LL[sq].count==0)
		{
		LL[sq].count = 1;
		LL[sq].elements = new Index[NINDEX];
		LL[sq].elements[0] = x;
		countLL++;
		int oldFirst = firstLL;
		firstLL = sq;
		LL[sq].prev = -1;
		LL[sq].next = oldFirst;
		if (oldFirst >= 0)
			{
			LL[oldFirst].prev = sq;
			};
		}
	else
		{
		LL[sq].elements[LL[sq].count++] = x;
		};
	};
	
printf("Initially created %d equivalence classes based on elements having the same square\n",countLL);

//	Modify the equivalence classes, so that x^2 itself is always in the equivalence class labelled by x^2, rather than (x^2)^2 if that is formally different

for (int pass=0;pass<2;pass++)
	{
	int ePtr = firstLL;
	int ec=0, nic=0, elc=0;
	while (ePtr >= 0)
		{
		ec++;
		if (pass==1) elc += LL[ePtr].count;
		int ecl = eqc[ePtr];
		if (ecl != ePtr)
			{
			nic++;
			
			//	Merge the ecl class into the ePtr class
			
			for (int k=0;k<LL[ecl].count;k++)
				{
				Index z = LL[ecl].elements[k];
				LL[ePtr].elements[LL[ePtr].count++] = z;
				eqc[z] = ePtr;
				};
				
			countLL--;
			
			//	Unlink
			
			int prev = LL[ecl].prev;
			int next = LL[ecl].next;
			if (prev<0) firstLL = next;
			else LL[prev].next = next;
			if (next>=0) LL[next].prev = prev;
			
			delete [] LL[ecl].elements;
			LL[ecl].elements = NULL;
			LL[ecl].count = 0;
			LL[ecl].prev = LL[ecl].next = -1;
			};
			
		ePtr = LL[ePtr].next;
		};
	printf("Equivalence classes checked = %d, those where x^2 not in own class = %d\n",ec,nic);
	printf("We now have %d equivalence classes\n",countLL);
	if (pass==1) printf("Total elements counted is %d\n",elc);
	};
	
//	Revalidate all equivalence class info

printf("Validating equivalence class info ...\n");
int ePtr = firstLL;
int eChk = 0;
while (ePtr >= 0)
	{
	for (int k=0;k<LL[ePtr].count;k++)
		{
		if (eqc[LL[ePtr].elements[k]] != ePtr)
			{
			printf("Failed\n");
			exit(EXIT_FAILURE);
			};
		};
	eChk += LL[ePtr].count;
	ePtr = LL[ePtr].next;
	};
printf("Done, total elements checked = %d\n\n",eChk);

//	We now check pairs of elements that are in the same equivalence class,
//	x1==x2, y1==y2, and if:
//
//	x1*y1 and x2*y2 are not in the same class
//	x1+y1 and x2+y2 are not in the same class
//
//	we merge the associated classes and start again

int *cnum = new int[countLL];

int passCount = 0;
while (true)
	{
	bool didMerge = false;
	int c1 = -1, c2 =-1;
	
	int ePtrX = firstLL;
	int nc = 0;
	while (ePtrX >= 0)
		{
		cnum[nc++] = ePtrX;
		ePtrX = LL[ePtrX].next;
		};
	qsort(cnum,nc,sizeof(cnum[0]),ecmp);
		
	for (int outerCount=0;outerCount<nc;outerCount++)
		{
		ePtrX = cnum[outerCount];
		
		printf("passCount = %d, outerCount = %d / %d, elements = %d\n",passCount, outerCount+1,countLL,LL[ePtrX].count);
		
		for (int k1=0;k1<LL[ePtrX].count;k1++)
			{
			Index x1 = LL[ePtrX].elements[k1];
			for (int k2=0;k2<LL[ePtrX].count;k2++)
				{
				Index x2 = LL[ePtrX].elements[k2];
				
				int ePtrY = firstLL;
				while (ePtrY >= 0)
					{
					for (int q1=0;q1<LL[ePtrY].count;q1++)
						{
						Index y1 = LL[ePtrY].elements[q1];
						for (int q2=0;q2<LL[ePtrY].count;q2++)
							{
							Index y2 = LL[ePtrY].elements[q2];
							
							c1 = eqc[MTAB[x1][y1]];
							c2 = eqc[MTAB[x2][y2]];
							if (c1!=c2) goto done;

							c1 = eqc[ATAB[x1][y1]];
							c2 = eqc[ATAB[x2][y2]];
							if (c1!=c2) goto done;
							};
						};
					ePtrY = LL[ePtrY].next;
					};
				};
			};
		};
		
done:

	if (c1 != c2)
		{
		printf("Merging classes ...\n");

		//	Merge the c2 class into the c1 class
		
		for (int k=0;k<LL[c2].count;k++)
			{
			Index z = LL[c2].elements[k];
			LL[c1].elements[LL[c1].count++] = z;
			eqc[z] = c1;
			};
			
		countLL--;
		
		//	Unlink
		
		int prev = LL[c2].prev;
		int next = LL[c2].next;
		if (prev<0) firstLL = next;
		else LL[prev].next = next;
		if (next>=0) LL[next].prev = prev;
		
		delete [] LL[c2].elements;
		LL[c2].elements = NULL;
		LL[c2].count = 0;
		LL[c2].prev = LL[c2].next = -1;

		didMerge = true;
		
		FILE *fp=fopen(OUTPUT_FILE,"wt");
		if (fp==NULL)
			{
			printf("Error opening output file %s to write\n",OUTPUT_FILE);
			printf("Sending output to console:\n");
			outputEC(stdout);
			}
		else
			{
			outputEC(fp);
			fclose(fp);
			};
		};
	
	if (!didMerge) break;
	passCount++;
	};

printf("We now have %d equivalence classes:\n",countLL);

return 0;
}

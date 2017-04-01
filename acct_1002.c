/*
	Author:		Alexandre Bobkov
	Started:	March 26, 2017
	Updated:	March 30, 2017

	Description:	
		- reads the accounting entries from the XML data file into linked list for further processing
		- populates accounts into linked list
		- calculates trial balance

	echo PATH=$PATH:~/Software/c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <math.h>
#include "ledger.h"
#include "gui.h"

#if defined(LIBXML_WRITER_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)

#define MY_ENCODING "UTF-8"
char *file_name = "/home/e403sa/Software/c/acct_proj/journal.xml";

int main(int argc, char **argv)
{
	JournalEntry **ptr, *start;

	void printEntries			(JournalEntry *);
	void printJournal			(JournalEntry **);
	void parseDoc				(char *,			JournalEntry *);
	void readJournal			(char *,			JournalEntry **);
	void parseEntry				(xmlDocPtr,			xmlNodePtr,		JournalEntry *);
	void readEntry				(xmlDocPtr,			xmlNodePtr,		JournalEntry **);
	void populateAccounts		(JournalEntry *,	JournalEntry *,	JournalEntry *);
	char *getDRAcctNum 			(JournalEntry *);
	JournalEntry* findAccount	(JournalEntry *, int);
	void addChartAccount		(JournalEntry *, int);
	void addChartAccount2		(JournalEntry *, JournalEntry *, int);
	JournalEntry* sortChartAccounts		(JournalEntry *);
	void insertChartAccount		(JournalEntry *, int);
	void getTrialBalance		(JournalEntry *, JournalEntry *);

	// Define structs.
	JournalEntry *je_xml, *je_head, *chart;
	// Initialize head entry of General Journal (head entry holds summary info about General Journal).
	je_xml = (JournalEntry*)malloc(sizeof(JournalEntry));
	strcpy (je_xml->entry_date,		"____GJ____");	// GJ - General Journal
	strcpy (je_xml->dr_acct_number,	"XXXX");		// no debit account
	strcpy (je_xml->cr_acct_number,	"XXXX");		// no credit account
	je_xml->dr_ttl = 0.00f;
	je_xml->cr_ttl = 0.00f;
	je_xml->next = NULL;
	je_xml->prev = NULL;
	// Set pointer to the address of head entry.
	ptr = &je_xml;
	
	// Initialize chart of accounts.
	chart = (JournalEntry*)malloc(sizeof(JournalEntry));
	strcpy (chart->entry_date, "____TB____");		// TB - Trial Balance
	chart->dr_acct = 0;
	chart->cr_acct = 0;
	chart->next = NULL;
	chart->prev = NULL;
	start = chart;

	// Output processed information.
	printf ("adresses dump:\n");
	printf ("**head:\t\t0x%X\n", 	*ptr);
	printf ("head:\t\t0x%X\n", 		je_xml);
	printf ("\nParsing entries from XML file.\n");

	// Read General Journal from XML datafile (double pointer).
	readJournal (file_name, ptr);
	je_xml->amount = ttl;		// save total entries amount into GJ head entry.

	// Print General Journal.
	printf ("\n");
	printf ("#   %s%sNode\tDate\t\tDR\tCR\t\t%10s", bold, white,"Amount\n");
	for (int n=0; n<115; n++)
		printf ("%s%s=%s", normal, yellow, none);
	printf ("\n");
	printEntries (*ptr);	// pass address from double pointer.
	// Print footer with total.
	printf ("%s%sTOTAL: \t%10.2f%s\n", bold, white, ttl, none);
	printf ("\n");
	
	//printf ("0x%X\n", je_xml);
	// send pointer.
	//printEntries(je_xml);	
	// send double pointer.
	//printJournal(ptr);

	// Create Chart of Accounts from General Journal entries.
	populateAccounts (start, je_xml, chart);

	//sortChartAccounts(chart);

	// Calculate trial balances for each account defined above.
	getTrialBalance (je_xml,chart);

	// Print Trial Balance.
	printf ("\nchart node: 0x%X\tnext: 0x%X\n", chart, chart->next);	
	for (int n=0; n<115; n++)
		printf ("%s%s=%s", normal, yellow, none);
	printf ("\n");
	printEntries (chart);

	for (int n=0; n<115; n++)
		printf ("%s%s=%s", normal, yellow, none);
	printf ("\n\n");
	printf ("%s%sEnding Balances%s\n", bold, white, none);

	JournalEntry *acct = chart;
	acct = sortChartAccounts(acct);
	float dr = 0.00f;
	float cr = 0.00f;

	do
	{
		acct = acct->next;
		dr += acct->dr_ttl;
		cr += acct->cr_ttl;
		//printf ("0x%X\t%i\t%10.2f\n", findAccount(chart, acct->dr_acct), acct->dr_acct, (findAccount(chart, acct->dr_acct)->dr_ttl - findAccount(chart, acct->cr_acct)->cr_ttl));
		printf ("0x%X\t%i\t%10.2f\n", acct, acct->dr_acct, (acct->dr_ttl - acct->cr_ttl));
	} while (acct->next != NULL);
	printf ("\n");
	printf ("%s%sTOTAL DEBITS:\t%10.2f\n", bold, white, dr, none);
	printf ("%s%sTOTAL CREDITS:\t%10.2f\n", bold, white, cr, none);
	printf ("%s%sTOTAL:\t%10.2f\n", bold, white, dr-cr, none);

	if ((float)(dr) - (float)(cr) == 0.00f)
		printf ("%s%sDEBITS AND CREDITS ARE IN BALANCE.%s\n", bold, green, none);
	else
		printf ("%s%sDEBITS DO NOT EQUAL CREDITS!%s\n", bold, red, none);	
}

#else
int main(void) {
    printf (stderr, "Writer or output support not compiled in\n");
    exit (1);
}
#endif

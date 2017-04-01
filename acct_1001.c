/*
	Author:	Alexandre Bobkov
	Date:	March 26, 2017
	Description:	parses accounting entries from XML data file into linked list for further processing.

	echo PATH=$PATH:~/Software/c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "ledger.h"
#include "gui.h"

#if defined (LIBXML_WRITER_ENABLED) && defined (LIBXML_OUTPUT_ENABLED)

#define MY_ENCODING "UTF-8"
char *file_name = "/home/e403sa/Software/c/acct_proj/journal.xml";

int main(int argc, char **argv)
{
	JournalEntry **head, *start;

	void printEntries	(JournalEntry *);
	void parseDoc		(char *, JournalEntry*);
	void parseJournal	(char *, JournalEntry **);
	void parseEntry		(xmlDocPtr, xmlNodePtr, JournalEntry *);
	void readEntry		(xmlDocPtr, xmlNodePtr, JournalEntry **);
	void populateAccounts	(JournalEntry *, JournalEntry *, JournalEntry*);
	char *getDRAcctNum	(JournalEntry *);
	JournalEntry* findAccount	(JournalEntry *, int);
	void addChartAccount	(JournalEntry *, int);
	void addChartAccount2	(JournalEntry *, JournalEntry*, int);
	void addEntry		(JournalEntry *, JournalEntry);
	void getTrialBalance	(JournalEntry *, JournalEntry *);

	// Initialize structs.
	JournalEntry *je_xml, *je_head, *chart;
	//je_xml = NULL;
	je_xml = (JournalEntry*)malloc(sizeof(JournalEntry));
	je_xml->next = NULL;
	je_xml->prev = NULL;

	chart = (JournalEntry*)malloc(sizeof(JournalEntry));
	chart->dr_acct=0.00;
	chart->next = NULL;
	chart->prev = NULL;
	start = chart;

	head=&je_xml;

	printf ("adresses dump:\n");
	printf ("**head:\t\t0x%X\n",	*head);
	printf ("head:\t\t0x%X\n",	je_xml);

	printf ("\nParsing entries from XML file.\n");

	parseDoc (file_name, je_xml);
	parseJournal (file_name, head);
	printf ("\n");
	printf ("#   %s%sNode\t\tDate\t\tDR\tCR\t%10s", bold, white,"Amount\n");
	for (int n=0; n<65; n++)
		printf ("%s%s=%s", normal, yellow, none);
	printf ("\n");
	strcpy (je_xml->entry_date,	"____JG____");
	strcpy (je_xml->dr_acct_number,	"XXXX");
	strcpy (je_xml->cr_acct_number,	"XXXX");

	je_xml->amount = ttl;
	printf ("0x%X\n", je_xml);
	printEntries (je_xml);
	printf ("%s%sTOTAL: \t%10.2f%s\n", bold, white, ttl, none);

	i = 0;
	printf ("\nchart node: 0x%X\tnext: 0x%X\n", chart, chart->next);
	strcpy (start->entry_date, "____TB____");
	populateAccounts (start, je_xml, chart);
	getTrialBalance (je_xml,chart);
	printEntries (chart);
}

#else
int main (void) {
    fprintf(stderr, "Writer or output support not compiled in\n");
    exit(1);
}
#endif

/*
* Themes: 
*	bright:	Monokai Bright; Mac Classic; Solorized Light;
*	dark:	Blackboard; Pastels on Dark; Monokai; Espresso Libre
*/

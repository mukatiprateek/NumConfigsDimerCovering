#include <iostream>
#include<vector>
#include <time.h>

//#include "bond.h"

using namespace std;

class Site;

class Bond {
	public:
	Site *site1;
	Site *site2;
	int present;// 1 if bond is present, 0 if absent

};

class Site{
	public:
	int siteindex;
	int nbonds;
	Site *prev;	// contained in numbonds array
	Site *next;
	vector<Bond> bonds;

	Site(int index){
		siteindex=index;
		nbonds=0;
		prev=NULL;
		next=NULL;

	};

};


class Lattice {

	public:

	int numSites;

	//TO DO : check this
	Site * numbonds[5];//eg : numbonds[1] will contain all the sites having one bond

	Site ** sites;//

	//constructer
	Lattice(int num){

		numSites=num;
		sites = new Site*[num];

		for (int i=0;i<5;i++)
		{
			numbonds[i]=NULL;
		};

		cout << "constructer finished \n";

	};

	//function
	void addSite(int index){

		if(!sites[index])
		{
		Site * site=new Site(index);

		sites[index]=site;
		}

		addSitetonumbonds(sites[index],0);

	}

	void deleteSite(int index){
		if(sites[index]->bonds.size()!=0)
		{
			cout << "Error : site can not be deleted since it has finite number of bonds\n";
		}
		else
		{
			removeSitefromnumBonds(sites[index],0);
		}

	}

	//function
	void addBond(int index1,int index2){
		Site * site1=sites[index1];
		Site * site2=sites[index2];
		int numbonds1=site1->bonds.size();
		int numbonds2=site2->bonds.size();

		//cout << "numbonds1 : " << numbonds1 << "\n";
		//cout << "numbonds2 : " << numbonds2 << "\n";

		Bond bond;
		bond.site1=site1;
		bond.site2=site2;

		//cout << "\n bond created \n";

		site1->bonds.push_back(bond);
		//cout << "numbonds1 : " << site1->bonds.size() << "\n";
		removeSitefromnumBonds(site1,numbonds1);
		addSitetonumbonds(site1,numbonds1+1);

		site2->bonds.push_back(bond);
		//cout << "numbonds2 : " << site2->bonds.size() << "\n";
		removeSitefromnumBonds(site2,numbonds2);
		addSitetonumbonds(site2,numbonds2+1);

	}

	void deleteBond(int index1,int index2){
		Site * site1=sites[index1];
		Site * site2=sites[index2];

		int numbonds1=site1->bonds.size();
		int numbonds2=site2->bonds.size();

	    for (auto i = site1->bonds.begin(); i != site1->bonds.end(); ++i) {
	        if (
	        		((*i).site1 == site1 && (*i).site2 == site2) ||
					((*i).site2 == site1 && (*i).site1 == site2)
	        	)
	        {
	        	site1->bonds.erase(i);
	    		removeSitefromnumBonds(site1,numbonds1);
	    		addSitetonumbonds(site1,numbonds1-1);
	    		break;
	        }
	    }

	    for (auto i = site2->bonds.begin(); i != site2->bonds.end(); ++i) {
	        if (
	        		((*i).site1 == site1 && (*i).site2 == site2) ||
					((*i).site2 == site1 && (*i).site1 == site2)
	        	)
	        {
	        	site2->bonds.erase(i);
	    		removeSitefromnumBonds(site2,numbonds2);
	    		addSitetonumbonds(site2,numbonds2-1);
	    		break;
	        }
	    }

	}

	void removeSitefromnumBonds(Site *st,int from){
		//cout << "Entering removeSitefromnumBonds : " << st->siteindex <<"\t"<<from<<"\n";
		Site * prevsite=st->prev;
		//cout << "prevsite : "<< prevsite << "\n";//<< "\t"<<prevsite->siteindex<<"\n";
		Site * nextsite=st->next;
		//cout << "nextsite : "<< nextsite <<"\n";//<< "\t" <<nextsite->siteindex<<"\n";

		if(!prevsite)//st is the head
		{
			//cout << "if : \n";
			numbonds[from]=nextsite;
			if(nextsite)
			{
				nextsite->prev=NULL;
			}
		}
		else
		{
			//cout << "else : \n";
			prevsite->next=nextsite;

			if(nextsite)
			{
				nextsite->prev=prevsite;
			}
		}

		st->prev=NULL;
		st->next=NULL;

		//cout <<"Exiting removeSitefromnumBonds";
	}
	void addSitetonumbonds(Site *st,int to){

		if(!numbonds[to]){

			//cout << "first site to be added\n";

			numbonds[to]=st;
		}
		else
		{
			Site * head=numbonds[to];
			st->next=head;
			head->prev=st;
			numbonds[to]=st;//site is new head
		}

	}
};


int Numconfigs(Lattice &latt){

	int numconfigs=0;

	if(latt.numbonds[0])//isolated site
		return 0;

	int numbondindex=-1;
	for(int i=1;i<=4;i++){
		if(latt.numbonds[i])
		{
			numbondindex=i;	//numbondindex corresponds to the site which will be covered using diff bonds one by one
			break;
		}
	}

	if(numbondindex==-1)//all the sites are covered
		return 1;

	vector<Bond> vect=latt.numbonds[numbondindex]->bonds;
    for (auto i = vect.begin(); i != vect.end(); ++i)
    {
    	//Backup of sites and bonds to be deleted to restore them later
        int siteindex1=(*i).site1->siteindex;
        int siteindex2=(*i).site2->siteindex;

        latt.deleteBond(siteindex1,siteindex2);

        vector<Bond> vector1=(*i).site1->bonds;
        vector<Bond> vector2=(*i).site2->bonds;

        for (auto j = vector1.begin(); j != vector1.end(); ++j){
        	latt.deleteBond((*j).site1->siteindex,(*j).site2->siteindex);
        }
        for (auto j = vector2.begin(); j != vector2.end(); ++j){
        	latt.deleteBond((*j).site1->siteindex,(*j).site2->siteindex);
        }

        latt.deleteSite(siteindex1);
        latt.deleteSite(siteindex2);

        //recursive call for sublattice after deletion
        numconfigs=numconfigs + Numconfigs(latt);

        //restoration of lattice

        latt.addSite(siteindex1);
        latt.addSite(siteindex2);
        latt.addBond(siteindex1,siteindex2);

        for (auto j = vector1.begin(); j != vector1.end(); ++j){
        	latt.addBond((*j).site1->siteindex,(*j).site2->siteindex);
        }
        for (auto j = vector2.begin(); j != vector2.end(); ++j){
        	latt.addBond((*j).site1->siteindex,(*j).site2->siteindex);
        }

    }

	return numconfigs;
}

Lattice prepareLattice(int x,int y){

cout << "Entering prepare lattice \n " ;


Lattice latt(x*y);

cout << "Adding sites \n " ;

	for(int i=0; i<x; i++){

	for(int j=0; j<y; j++){

		int index=i*y+j;

		latt.addSite(index);//call addsite

	}

	}

cout << "sites added\n " ;

cout << "adding bonds\n";

latt.addBond(0,1);

latt.addBond(0,2);
latt.addBond(1,3);
latt.addBond(2,3);

latt.addBond(2,4);
latt.addBond(3,5);
latt.addBond(4,5);

latt.addBond(4,6);
latt.addBond(5,7);
latt.addBond(6,7);

latt.addBond(6,8);
latt.addBond(7,9);
latt.addBond(8,9);

latt.addBond(8,10);
latt.addBond(9,11);
latt.addBond(10,11);

latt.addBond(0,10);
latt.addBond(1,11);

cout << "bonds added\n";

cout << "Exiting prepare lattice \n ";

return latt;

};

Lattice preparePeriodicLattice(int x,int y){

cout << "Entering preparePeriodicLattice \n " ;

Lattice latt(x*y);

cout << "Adding sites \n " ;

	for(int i=0; i<x; i++){

	for(int j=0; j<y; j++){

		int index=i*y+j;

		latt.addSite(index);//call addsite

	}

	}

	cout << "Adding bonds \n";

	for(int i=0; i<x; i++){

	for(int j=0; j<y; j++){

		int index1=i*y+j;

		int index2=((i+1)%x)*y+j;//right bond
		latt.addBond(index1,index2);
		//cout << index1 << index2 << "\n";

		index2=i*y+(j+1)%y;//up bond
		latt.addBond(index1,index2);
		//cout << index1 << index2 << "\n";
	}

	}

	cout << "bonds added \n";

	cout << "Exiting preparePeriodicLattice \n ";

	return latt;

}

//TODO : implement swap function to clarify things regarding pass by value,pass by refrence, *,& etc

int main(){

    clock_t tStart = clock();

	int nx=8;
	int ny=8;

	//Lattice lattice=prepareLattice(nx,ny);

	Lattice lattice = preparePeriodicLattice(nx,ny);

	int numconfigs=Numconfigs(lattice);

	cout << " \n numconfigs : " << numconfigs << "\t";

    clock_t tEnd = clock();

    printf("Time taken: %.2fs\n", (double)(tEnd - tStart)/CLOCKS_PER_SEC);

	//cout << "numbonds[0] : " << lattice.numbonds[0]->siteindex <<"\t" << lattice.numbonds[0]->next->siteindex << "\n";
	//cout << "numbonds[1] : " << lattice.numbonds[1]->bonds.size()<<"\n";
	//cout << "numbonds[1] : " << lattice.numbonds[1]->siteindex <<"\t" << lattice.numbonds[1]->next->siteindex << "\n";
	//cout << "numbonds[2] : " << lattice.numbonds[2]->bonds.size()<<"\n";
};

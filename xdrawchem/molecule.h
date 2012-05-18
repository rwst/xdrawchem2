// molecule.h -- subclass of Drawable which contains a molecule

#ifndef MOLECULE_H
#define MOLECULE_H

#include "drawable.h"
#include "dpoint.h"
#include "bond.h"
#include "text.h"
#include "molecule_sssr.h"
#include "tooldialog.h"
#include "peak.h"
#include "defs.h"

#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include "symbol.h"

class ChemData;
class Render2D;

class Molecule : public Drawable
{
Q_OBJECT

public:
    Molecule();
    Molecule (Molecule*);
    ~Molecule();
    void Render(Render2D * r);  // draw this object
    void Edit( Render2D* /* r */ ) {}
    void Move( double, double );
    void Rotate( DPoint *, double );
    void Flip( DPoint *, int );
    void Resize( DPoint *, double );
    const QRect BoundingBox() const;
    QString ToXML( QString );
    QString ToCDXML( QString );
    void FromXML( QString );

    bool Find( DPoint * );   // is this DPoint present in this Molecule?
    DPoint *FindNearestPoint( DPoint *, double & );
    double distanceTo( DPoint * );
    DPoint *End() { return end; }

    Render2D *getRender2D() { return r; }
    void addBond( DPoint *, DPoint *, int, int, QColor, bool hl = false );
    void addBond( QSharedPointer<Bond> );
    void addText( QSharedPointer<Text> );
    void addSymbol( QSharedPointer<Symbol> );
    void CopyTextToDPoint();
    void CalcOffsets();
    void addMolecule( Molecule * );

    bool Erase(QSharedPointer<Drawable> );
    void EraseSelected();
    bool isWithinRect( QRect, bool );
    bool WithinBounds( DPoint * );
    void SelectAll();
    void DeselectAll();
    void SetColorIfHighlighted( QColor );
    void Rotate( double );
    QRect BoundingBoxAll();
    QList<DPoint *> AllPoints();
    QList<QSharedPointer<Drawable> > AllObjects();
    const QList<QSharedPointer<Bond> > AllBonds();
    QList<QSharedPointer<Molecule> > MakeSplit();
    int Members() { return bonds.count(); }
    QString ToMDLMolfile( int coords = 0 );
    void Changed();
    void doChanged();
    // defined in molecule_tools.cpp
    QSharedPointer<Bond> FindBond( DPoint *, DPoint * );
    int OrderOfBond( DPoint *, DPoint * );
    void Reactivity( int );  // molecule_tools_2.cpp for now
    QList<DPoint *> BreakRingBonds( DPoint * );
    DPoint *GetRingAttachPoint();
    DPoint *GetAttachPoint(QString);
    double CalculateRingAttachAngle( DPoint * );
    void FindHybridization();
    void setGroupType( int );
    Text *CalcMW( bool from_change = false );
    Text *CalcEmpiricalFormula( bool from_mw = false );
    Text *CalcElementalAnalysis( bool show_dialog = true );
    QStringList Calc13CNMR( bool show_dialog = true );
    void CalcIR();
    QString CalcName();  // actually returns canonical SMILES
    //QString GetCASNumber();
    //QString IUPAC_Name();
    void AllNeighbors();
    void MakeSSSR();
    void AddHydrogens(bool to_carbon = false);
    double SumBondEnthalpy();
    void Scale(double bond_length = -1.0);
    void AddPeak(double, QString, QString);
    void Make3DVersion(QString fn3d = "");
    void CalcpKa();
    double CalcPartialCharge(QString);
    double CalcKOW();
    // defined in molecule_1h_nmr.cpp
    QStringList Calc1HNMR(bool show_dialog = true);
    void AddNMRprotons();
    void RemoveNMRprotons();
    void ProtonEnvironment();
    void Multiplicity_1HNMR();
    void Calc1HMultiplicityAndIntensity();
    double Calc1HShift(QString);
    // defined in molecule_smiles.cpp
    void CleanUp();
    void SDG(bool);
    QString ToSMILES();
    void FromSMILES(QString);
    QString ToInChI();
    void FromInChI(QString);
    // defined in retro.cpp
    QString BuildReactionList(QString);
    int Retro();
    QString RetroTraverseBonds(DPoint *, DPoint *, QSharedPointer<Bond> , int);
    QString RetroAtomName(DPoint *);
    QString RetroBondName(QSharedPointer<Bond>, bool runsssr = false);
    bool RetroMatch(QString, QString);
    bool RetroTreeMatch(QString, QString, QString, QString);
    // defined here:
    bool isInGroup()
    {
        if (group_type != GROUP_NONE) return true;
        return false;
    }
    int groupType() { return group_type; }
    void FormulaLabelDeleted() { qDebug() << "FLD" ; text_formula = 0; }
    void MWLabelDeleted() { text_mw = 0; }
    double Angle(QSharedPointer<Bond> &a1, QSharedPointer<Bond> &b1)
    {
        // from Bond a1 to Bond b1
        // determine endpoints
        DPoint *a, *c, *b;
        if (a1->Start() == b1->Start()) {
            c = a1->Start();
            a = a1->End();
            b = b1->End();
        }
        if (a1->Start() == b1->End()) {
            c = a1->Start();
            a = a1->End();
            b = b1->Start();
        }
        if (a1->End() == b1->Start()) {
            c = a1->End();
            b = a1->Start();
            a = b1->End();
        }
        if (a1->End() == b1->End()) {
            c = a1->End();
            b = a1->Start();
            a = b1->Start();
        }
        double ang1 = getAngle(c, a);
        double ang2 = getAngle(c, b);
        double ang3 = ang1 - ang2;
        if (ang3 < 0.0) ang3 += 360.0;
        return ang3;
    }

    OpenBabel::OBMol * convertToOBMol();
    bool convertFromOBMol(OpenBabel::OBMol *);

    // not appropriate but best way to handle ToolDialog requests.
    QList<Peak *> peaklist;
    Peak *tmp_peak;
    QList<QSharedPointer<Bond> > bonds;
    QList<QSharedPointer<Text> > labels;
    QList<QSharedPointer<Symbol> > symbols;

    friend QDebug operator<<(QDebug dbg, const Molecule &m);

private:
    // Renderer
    Render2D *r;
    // Text objects which hold MW and formula
    Text *text_mw, *text_formula;
    // used for elemental analysis (set by CalcEmpiricalFormula)
    double nc, nh, no, nn, nmw;
    // Molecule's SSSR
    SSSR this_sssr;
    // point list for SDG, etc.
    QList<DPoint *> up;
    // group data
    int group_type;
    int group_id;
    // proton magnetic environment list(s)
    QStringList protonMagEnvList;
    QStringList protonFinalList;
    // render partial charges?
    bool showPC, changed;
    // reactivity info
    QStringList atomRxns, bondRxns;
    DPoint *end;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;

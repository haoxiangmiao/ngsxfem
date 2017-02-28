/// from ngxfem
#include "../xfem/cutinfo.hpp"
#include "../cutint/xintegration.hpp"
using namespace ngsolve;
using namespace xintegration;

namespace ngcomp
{
  CutInformation::CutInformation (shared_ptr<MeshAccess> ama)
    : ma(ama),
    cut_ratio_of_element(2),
    elems_of_domain_type(3),
    selems_of_domain_type(3),
    facets_of_domain_type(3)
  {
    for (auto dt : {NEG,POS,IF})
    {
      elems_of_domain_type[dt] = make_shared<BitArray>(ma->GetNE(VOL));
      selems_of_domain_type[dt] = make_shared<BitArray>(ma->GetNE(BND));
      facets_of_domain_type[dt] = make_shared<BitArray>(ma->GetNFacets());
    }
    facets_of_domain_type[NEG]->Set();
    facets_of_domain_type[POS]->Clear();
    facets_of_domain_type[IF]->Clear();
  }

  void CutInformation::Update(shared_ptr<CoefficientFunction> lset, LocalHeap & lh)
  {
    // // NODE_TYPE: NT_VERTEX
    // int nv = ma->GetNV();
    // cut_ratio_of_node[NT_VERTEX] = make_shared<VVector<double>>(nv);
    // // ...

    // // NODE_TYPE: NT_EDGE
    // int nedges = ma->GetNEdges();
    // cut_ratio_of_node[NT_EDGE] = make_shared<VVector<double>>(nedges);
    // // ...

    // // NODE_TYPE: NT_FACE
    // int nfaces = ma->GetNFaces();
    // cut_ratio_of_node[NT_FACE] = make_shared<VVector<double>>(nfaces);
    // // ...

    // // NODE_TYPE: NT_CELL
    // int ncells = ma->GetDimension() == 3 ? ma->GetNE() : 0;
    // cut_ratio_of_node[NT_CELL] = make_shared<VVector<double>>(ncells);
    // // ...
    for (auto dt : {NEG,POS,IF})
    {
      elems_of_domain_type[dt]->Clear();
      selems_of_domain_type[dt]->Clear();
    }

    for (VorB vb : {VOL,BND})
    {
      int ne = ma->GetNE(vb);
      cut_ratio_of_element[vb] = make_shared<VVector<double>>(ne);
      IterateRange
        (ne, lh,
        [&] (int elnr, LocalHeap & lh)
      {
        ElementId ei = ElementId(vb,elnr);
        Ngs_Element ngel = ma->GetElement(ei);
        ELEMENT_TYPE eltype = ngel.GetType();
        ElementTransformation & eltrans = ma->GetTrafo (ei, lh);
        ScalarFieldEvaluator * lset_eval_p = ScalarFieldEvaluator::Create(ma->GetDimension(),
                                                                          *lset,eltrans,lh);
        double part_vol [] = {0.0, 0.0};
        for (DOMAIN_TYPE np : {POS, NEG})
        {
          const IntegrationRule * ir_np = CutIntegrationRule(lset, eltrans, np, 0, subdivlvl,lh);
          if (ir_np)
            for (auto ip : *ir_np)
              part_vol[np] += ip.Weight();
        }
        (*cut_ratio_of_element[vb])(elnr) = part_vol[NEG]/(part_vol[NEG]+part_vol[POS]);

        if (vb == VOL)
        {
          if (part_vol[NEG] > 0.0)
            if (part_vol[POS] > 0.0)
              (*elems_of_domain_type[IF]).Set(elnr);
            else
              (*elems_of_domain_type[NEG]).Set(elnr);
          else
            (*elems_of_domain_type[POS]).Set(elnr);
        }
        else
        {
          if (part_vol[NEG] > 0.0)
            if (part_vol[POS] > 0.0)
              (*selems_of_domain_type[IF]).Set(elnr);
            else
              (*selems_of_domain_type[NEG]).Set(elnr);
          else
            (*selems_of_domain_type[POS]).Set(elnr);
        }

      });
    }
  }
}

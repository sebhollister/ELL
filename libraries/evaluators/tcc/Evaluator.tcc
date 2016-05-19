////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Evaluator.tcc (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryErrorAggregator.h"

namespace evaluators
{
    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    Evaluator<PredictorType, EvaluationAggregatorTupleType>::Evaluator(dataset::GenericRowDataset::Iterator exampleIterator, EvaluationAggregatorTupleType evaluationAggregatorTuple)
        : _rowDataset(exampleIterator), _evaluationAggregatorTuple(std::move(evaluationAggregatorTuple)) 
    {
        static_assert(std::tuple_size<EvaluationAggregatorTupleType>::value > 0, "Evaluator must contains at least one aggregator");
    }

    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    void Evaluator<PredictorType, EvaluationAggregatorTupleType>::Evaluate(const PredictorType& predictor)
    {
        auto iterator = _rowDataset.GetIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetWeight();
            double label = example.GetLabel();
            double prediction = predictor.Predict(example.GetDataVector());

            DispatchUpdate(prediction, label, weight, std::make_index_sequence<std::tuple_size<EvaluationAggregatorTupleType>::value>());
            iterator.Next();
        }
    }

    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    void Evaluator<PredictorType, EvaluationAggregatorTupleType>::Print(std::ostream& os) const
    {
        DispatchPrint(os, std::make_index_sequence<std::tuple_size<decltype(_evaluationAggregatorTuple)>::value>());
    }

    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    template<std::size_t ...Is>
    void Evaluator<PredictorType, EvaluationAggregatorTupleType>::DispatchUpdate(double prediction, double label, double weight, std::index_sequence<Is...>)
    {
        // Call X.Update() for each X in _evaluationAggregatorTuple
        auto dummy = {(std::get<Is>(_evaluationAggregatorTuple).Update(prediction, label, weight), 0)...}; // OMG!
    }

    template<typename PredictorType, typename EvaluationAggregatorTupleType>
    template<std::size_t ...Is>
    void Evaluator<PredictorType, EvaluationAggregatorTupleType>::DispatchPrint(std::ostream& os, std::index_sequence<Is...>) const
    {
        // Call X.ToString() for each X in _evaluationAggregatorTuple
        std::vector<std::string> strings {std::get<Is>(_evaluationAggregatorTuple).ToString()...};

        os << strings[0];
        for(uint64_t i=1; i<strings.size(); ++i)
        {
            os << "\t" << strings[i];
        }
        os << std::endl;
    }

    template<typename PredictorType, typename... EvaluationAggregatorTupleType>
    std::unique_ptr<IEvaluator<PredictorType>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, EvaluationAggregatorTupleType... evaluationAggregatorTuple)
    {
        return std::make_unique<Evaluator<PredictorType, std::tuple<EvaluationAggregatorTupleType...>>>(exampleIterator, std::make_tuple(evaluationAggregatorTuple...));
    }

    //template<typename PredictorType, typename... AggregatorTypes>
    //Evaluator<PredictorType, std::tuple<AggregatorTypes...>> MakeEvaluator(dataset::GenericRowDataset::Iterator exampleIterator, AggregatorTypes... evaluationAggregatorTuple)
    //{
    //    using EvaluationAggregatorTupleType = decltype(std::make_tuple(evaluationAggregatorTuple...));
    //    EvaluationAggregatorTupleType tuple = std::make_tuple(evaluationAggregatorTuple...);
    //    return Evaluator<PredictorType, EvaluationAggregatorTupleType>(exampleIterator, std::move(tuple));
    //}

}
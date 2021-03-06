// Copyright 2017 Yahoo Holdings. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include "multistringattribute.h"
#include "postinglistattribute.h"
#include "i_document_weight_attribute.h"

namespace search {

/**
 * Implementation of multi value string attribute that in addition to enum store and
 * multi value mapping uses an underlying posting list to provide faster search.
 * This class is used for both array and weighted set types.
 *
 * B: EnumAttribute<StringAttribute>
 * T: multivalue::Value<IEnumStore::Index> (array) or
 *    multivalue::WeightedValue<IEnumStore::Index> (weighted set)
 */
template <typename B, typename T>
class MultiValueStringPostingAttributeT
    : public MultiValueStringAttributeT<B, T>,
      protected PostingListAttributeSubBase<AttributeWeightPosting,
                                            typename B::LoadedVector,
                                            typename B::LoadedValueType,
                                            typename B::EnumStore>
{
public:
    using EnumStore = typename MultiValueStringAttributeT<B, T>::EnumStore;
    using EnumStoreBatchUpdater = typename EnumStore::BatchUpdater;

private:
    struct DocumentWeightAttributeAdapter : IDocumentWeightAttribute {
        const MultiValueStringPostingAttributeT &self;
        DocumentWeightAttributeAdapter(const MultiValueStringPostingAttributeT &self_in) : self(self_in) {}
        virtual LookupResult lookup(const vespalib::string &term) const override final;
        virtual void create(vespalib::datastore::EntryRef idx, std::vector<DocumentWeightIterator> &dst) const override final;
        virtual DocumentWeightIterator create(vespalib::datastore::EntryRef idx) const override final;
    };
    DocumentWeightAttributeAdapter _document_weight_attribute_adapter;

    friend class PostingListAttributeTest;
    template <typename, typename, typename> 
    friend class attribute::PostingSearchContext; // getEnumStore()
    friend class StringAttributeTest;

    using LoadedVector = typename B::LoadedVector;
    using PostingParent = PostingListAttributeSubBase<AttributeWeightPosting,
                                                      LoadedVector,
                                                      typename B::LoadedValueType,
                                                      typename B::EnumStore>;

    using ComparatorType = typename EnumStore::ComparatorType;
    using Dictionary = EnumPostingTree;
    using DictionaryConstIterator = typename Dictionary::ConstIterator;
    using DocId = typename MultiValueStringAttributeT<B, T>::DocId;
    using DocIndices = typename MultiValueStringAttributeT<B, T>::DocIndices;
    using EnumIndex = typename EnumStore::Index;
    using FoldedComparatorType = typename EnumStore::FoldedComparatorType;
    using FrozenDictionary = typename Dictionary::FrozenView;
    using LoadedEnumAttributeVector = attribute::LoadedEnumAttributeVector;
    using Posting = typename PostingParent::Posting;
    using PostingList = typename PostingParent::PostingList;
    using PostingMap = typename PostingParent::PostingMap;
    using QueryTermSimpleUP = AttributeVector::QueryTermSimpleUP;
    using SelfType = MultiValueStringPostingAttributeT<B, T>;
    using StringArrayImplSearchContext = typename MultiValueStringAttributeT<B, T>::StringArrayImplSearchContext;
    using StringArrayPostingSearchContext = attribute::StringPostingSearchContext<StringArrayImplSearchContext, SelfType, int32_t>;
    using StringSetImplSearchContext = typename MultiValueStringAttributeT<B, T>::StringSetImplSearchContext;
    using StringSetPostingSearchContext = attribute::StringPostingSearchContext<StringSetImplSearchContext, SelfType, int32_t>;
    using WeightedIndex = typename MultiValueStringAttributeT<B, T>::WeightedIndex;
    using generation_t = typename MultiValueStringAttributeT<B, T>::generation_t;

    using PostingParent::_postingList;
    using PostingParent::clearAllPostings;
    using PostingParent::handle_load_posting_lists;
    using PostingParent::handle_load_posting_lists_and_update_enum_store;
    using PostingParent::forwardedOnAddDoc;

    void freezeEnumDictionary() override;
    void mergeMemoryStats(vespalib::MemoryUsage & total) override;
    void applyValueChanges(const DocIndices& docIndices, EnumStoreBatchUpdater& updater) override ;

public:
    MultiValueStringPostingAttributeT(const vespalib::string & name, const AttributeVector::Config & c =
                                      AttributeVector::Config(AttributeVector::BasicType::STRING,
                                                              attribute::CollectionType::ARRAY));
    ~MultiValueStringPostingAttributeT();

    void removeOldGenerations(generation_t firstUsed) override;
    void onGenerationChange(generation_t generation) override;

    AttributeVector::SearchContext::UP
    getSearch(QueryTermSimpleUP term, const attribute::SearchContextParams & params) const override;

    const IDocumentWeightAttribute *asDocumentWeightAttribute() const override;

    bool onAddDoc(DocId doc) override {
        return forwardedOnAddDoc(doc, this->_mvMapping.getNumKeys(), this->_mvMapping.getCapacityKeys());
    }
    
    void load_posting_lists(LoadedVector& loaded) override {
        handle_load_posting_lists(loaded);
    }

    attribute::IPostingListAttributeBase * getIPostingListAttributeBase() override { return this; }

    const attribute::IPostingListAttributeBase * getIPostingListAttributeBase()  const override { return this; }

    void load_posting_lists_and_update_enum_store(enumstore::EnumeratedPostingsLoader& loader) override {
        handle_load_posting_lists_and_update_enum_store(loader);
    }
};

using ArrayStringPostingAttribute = MultiValueStringPostingAttributeT<EnumAttribute<StringAttribute>, multivalue::Value<IEnumStore::Index> >;
using WeightedSetStringPostingAttribute = MultiValueStringPostingAttributeT<EnumAttribute<StringAttribute>, multivalue::WeightedValue<IEnumStore::Index> >;

} // namespace search


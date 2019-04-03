#pragma once

#include "pch.h"
#include "BackgroundImage.h"
#include "BaseCardElement.h"

namespace AdaptiveSharedNamespace
{
    class CollectionTypeElement : public BaseCardElement
    {
    public:
        CollectionTypeElement(const CollectionTypeElement&) = default;
        CollectionTypeElement(CollectionTypeElement&&) = default;
        CollectionTypeElement& operator=(const CollectionTypeElement&) = default;
        CollectionTypeElement& operator=(CollectionTypeElement&&) = default;
        ~CollectionTypeElement() = default;
        CollectionTypeElement(CardElementType type, ContainerStyle style = ContainerStyle::None,
            VerticalContentAlignment alignment = VerticalContentAlignment::Top);

        ContainerStyle GetStyle() const;
        void SetStyle(const ContainerStyle value);

        VerticalContentAlignment GetVerticalContentAlignment() const;
        void SetVerticalContentAlignment(const VerticalContentAlignment value);

        bool GetPadding() const;

        bool GetBleed() const;
        void SetBleed(const bool value);
        // tells if current element can bleed
        // when GetCanBleed() return true
        // Can Bleed
        // To find the direction of the bleed
        // Call GetBleedDirection()        
        // the renderer must also check if HostConfig has
        // padding for card, the root, if the padding is allowed,
        // then the element can bleed to the card
        bool GetCanBleed() const { return m_canBleed; }
        // 1. BleedToLeading: bleed its leading edge to the leading edge of the target parent
        // 2. BleedToTrailing: bleed its trailing edge to the trailing edge of the target parent
        // 3. RestrictedInAllDrections: doesn't bleed
        // 4. BleedToBothEdges: bleed to both edges of the target parent
        ContainerBleedDirection GetBleedDirection() const { return m_bleedDirection; }

        // configures container style related attributes
        // such as style, padding and bleed
        void ConfigForContainerStyle(const AdaptiveCards::ParseContext& context);

        void SetParentalId(const AdaptiveSharedNamespace::InternalId &id);
        AdaptiveSharedNamespace::InternalId GetParentalId(void) const;

        std::shared_ptr<BaseActionElement> GetSelectAction() const;
        void SetSelectAction(const std::shared_ptr<BaseActionElement> action);

        std::shared_ptr<BackgroundImage> GetBackgroundImage() const;
        void SetBackgroundImage(const std::shared_ptr<BackgroundImage> value);

        template<typename T>
        void GetResourceInformation(std::vector<RemoteResourceInformation>& resourceInfo,
                                    const std::vector<std::shared_ptr<T>>& elements);

        virtual void DeserializeChildren(AdaptiveCards::ParseContext& context, const Json::Value& value) = 0;

        Json::Value SerializeToJsonValue() const override;
        template<typename T> static std::shared_ptr<T> Deserialize(ParseContext& context, const Json::Value& value);

    private:
        void SetCanBleed(const bool value) { m_canBleed = value;}
        void SetPadding(const bool value);

        // Applies padding flag When appropriate
        void ConfigPadding(const AdaptiveCards::ParseContext& context);
        // Applies bleed flag when appropriate
        void ConfigBleed(const AdaptiveCards::ParseContext& context);
        void SetBleedDirection(const ContainerBleedDirection bleedDirection) { m_bleedDirection = bleedDirection; }

        ContainerStyle m_style;
        VerticalContentAlignment m_verticalContentAlignment;
        bool m_canBleed;
        ContainerBleedDirection m_bleedDirection;

        bool m_hasPadding;
        bool m_hasBleed;
        // id refers to parent to where bleed property should target
        AdaptiveSharedNamespace::InternalId m_parentalId;

        std::shared_ptr<BackgroundImage> m_backgroundImage;
        std::shared_ptr<BaseActionElement> m_selectAction;
    };

    template<typename T>
    std::shared_ptr<T> CollectionTypeElement::Deserialize(ParseContext& context, const Json::Value& value)
    {
        auto collection = BaseCardElement::Deserialize<T>(context, value);

        auto backgroundImage = ParseUtil::GetBackgroundImage(value);
        collection->SetBackgroundImage(backgroundImage);

        collection->SetStyle(
            ParseUtil::GetEnumValue<ContainerStyle>(value, AdaptiveCardSchemaKey::Style, ContainerStyle::None, ContainerStyleFromString));

        collection->SetVerticalContentAlignment(ParseUtil::GetEnumValue<VerticalContentAlignment>(
            value, AdaptiveCardSchemaKey::VerticalContentAlignment, VerticalContentAlignment::Top, VerticalContentAlignmentFromString));

        collection->SetBleed(ParseUtil::GetBool(value, AdaptiveCardSchemaKey::Bleed, false));

        // configures for cotainer style
        collection->ConfigForContainerStyle(context);

        // we walk parse tree dfs, so we need to save current style,
        // before we walk back up to a parent.
        context.SaveContextForCollectionTypeElement(collection);

        // Parse Items
        collection->DeserializeChildren(context, value);

        // since we are walking dfs, we have to restore the style before we back up
        context.RestoreContextForCollectionTypeElement(collection);

        // Parse optional selectAction
        collection->SetSelectAction(ParseUtil::GetAction(context, value, AdaptiveCardSchemaKey::SelectAction, false));

        return collection;
    }

    template<typename T>
    void CollectionTypeElement::GetResourceInformation(std::vector<RemoteResourceInformation>& resourceInfo,
                                                       const std::vector<std::shared_ptr<T>>& elements)
    {
        if (m_backgroundImage != nullptr)
        {
            RemoteResourceInformation backgroundImageInfo;
            backgroundImageInfo.url = m_backgroundImage->GetUrl();
            backgroundImageInfo.mimeType = "image";
            resourceInfo.push_back(backgroundImageInfo);
        }

        for (const auto& elem : elements)
        {
            elem->GetResourceInformation(resourceInfo);
        }
    }
}

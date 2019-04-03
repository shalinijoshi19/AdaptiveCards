#include "pch.h"
#include "AdaptiveColumnSet.h"

#include "Util.h"
#include "Vector.h"
#include <windows.foundation.collections.h>
#include "XamlHelpers.h"
#include "AdaptiveColumn.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Controls;

namespace AdaptiveNamespace
{
    AdaptiveColumnSet::AdaptiveColumnSet()
    {
        m_columns = Microsoft::WRL::Make<Vector<ABI::AdaptiveNamespace::AdaptiveColumn*>>();
    }

    HRESULT AdaptiveColumnSet::RuntimeClassInitialize() noexcept try
    {
        std::shared_ptr<AdaptiveSharedNamespace::ColumnSet> columnSet = std::make_shared<AdaptiveSharedNamespace::ColumnSet>();
        return RuntimeClassInitialize(columnSet);
    }
    CATCH_RETURN;

    HRESULT AdaptiveColumnSet::RuntimeClassInitialize(const std::shared_ptr<AdaptiveSharedNamespace::ColumnSet>& sharedColumnSet) try
    {
        if (sharedColumnSet == nullptr)
        {
            return E_INVALIDARG;
        }

        GenerateColumnsProjection(sharedColumnSet->GetColumns(), m_columns.Get());
        GenerateActionProjection(sharedColumnSet->GetSelectAction(), &m_selectAction);

        m_style = static_cast<ABI::AdaptiveNamespace::ContainerStyle>(sharedColumnSet->GetStyle());
        m_bleed = sharedColumnSet->GetBleed();

        InitializeBaseElement(std::static_pointer_cast<BaseCardElement>(sharedColumnSet));

        return S_OK;
    }
    CATCH_RETURN;

    IFACEMETHODIMP AdaptiveColumnSet::get_Columns(_COM_Outptr_ IVector<ABI::AdaptiveNamespace::AdaptiveColumn*>** columns)
    {
        return m_columns.CopyTo(columns);
    }

    IFACEMETHODIMP AdaptiveColumnSet::get_SelectAction(_COM_Outptr_ IAdaptiveActionElement** action)
    {
        return m_selectAction.CopyTo(action);
    }

    IFACEMETHODIMP AdaptiveColumnSet::put_SelectAction(_In_ IAdaptiveActionElement* action)
    {
        m_selectAction = action;
        return S_OK;
    }

    HRESULT AdaptiveColumnSet::get_Style(_Out_ ABI::AdaptiveNamespace::ContainerStyle* style)
    {
        *style = m_style;
        return S_OK;
    }

    HRESULT AdaptiveColumnSet::put_Style(ABI::AdaptiveNamespace::ContainerStyle style)
    {
        m_style = style;
        return S_OK;
    }

    HRESULT AdaptiveColumnSet::get_Bleed(_Out_ boolean* isSubtle)
    {
        *isSubtle = m_bleed;
        return S_OK;
    }

    HRESULT AdaptiveColumnSet::put_Bleed(boolean isSubtle)
    {
        m_bleed = isSubtle;
        return S_OK;
    }

    IFACEMETHODIMP AdaptiveColumnSet::get_ElementType(_Out_ ElementType* elementType)
    {
        *elementType = ElementType::ColumnSet;
        return S_OK;
    }

    HRESULT AdaptiveColumnSet::GetSharedModel(std::shared_ptr<AdaptiveSharedNamespace::BaseCardElement>& sharedModel) try
    {
        std::shared_ptr<AdaptiveSharedNamespace::ColumnSet> columnSet = std::make_shared<AdaptiveSharedNamespace::ColumnSet>();
        RETURN_IF_FAILED(SetSharedElementProperties(std::static_pointer_cast<AdaptiveSharedNamespace::BaseCardElement>(columnSet)));

        if (m_selectAction != nullptr)
        {
            std::shared_ptr<BaseActionElement> sharedAction;
            RETURN_IF_FAILED(GenerateSharedAction(m_selectAction.Get(), sharedAction));
            columnSet->SetSelectAction(sharedAction);
        }

        XamlHelpers::IterateOverVector<ABI::AdaptiveNamespace::AdaptiveColumn, IAdaptiveColumn>(m_columns.Get(), [&](IAdaptiveColumn* column) {
            ComPtr<AdaptiveNamespace::AdaptiveColumn> columnImpl = PeekInnards<AdaptiveNamespace::AdaptiveColumn>(column);

            std::shared_ptr<BaseCardElement> sharedColumnBaseElement;
            RETURN_IF_FAILED(columnImpl->GetSharedModel(sharedColumnBaseElement));

            std::shared_ptr<AdaptiveSharedNamespace::Column> sharedColumn =
                std::AdaptivePointerCast<AdaptiveSharedNamespace::Column>(sharedColumnBaseElement);
            if (sharedColumn == nullptr)
            {
                return E_UNEXPECTED;
            }

            columnSet->GetColumns().push_back(sharedColumn);

            return S_OK;
        });

        columnSet->SetStyle(static_cast<AdaptiveSharedNamespace::ContainerStyle>(m_style));
        columnSet->SetBleed(m_bleed);

        sharedModel = columnSet;
        return S_OK;
    }
    CATCH_RETURN;
}

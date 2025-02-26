// Glaze Library
// For the license information refer to glaze.hpp

#pragma once

#include <algorithm>
#include <string_view>

#include "glaze/core/meta.hpp"
#include "glaze/util/for_each.hpp"
#include "glaze/util/string_literal.hpp"
#include "glaze/util/type_traits.hpp"

namespace glz
{
   namespace detail
   {
#ifdef _MSC_VER
      // Workaround for problems with MSVC and passing refrences to stringviews as template params
      struct svw
      {
         const char* start{};
         size_t n{};
         constexpr svw(std::string_view sv) : start(sv.data()), n(sv.size()) {}
         constexpr auto data() const { return start; }
         constexpr auto begin() const { return data(); }
         constexpr auto end() const { return data() + n; }
         constexpr auto size() const { return n; }
      };
      template <svw... Strs>
#else
      template <const std::string_view&... Strs>
#endif
      struct join
      {
         // Join all strings into a single std::array of chars
         static constexpr auto impl() noexcept
         {
            // This local copy to a tuple and avoiding of parameter pack expansion is needed to avoid MSVC internal
            // compiler errors
            constexpr size_t len = (Strs.size() + ... + 0);
            std::array<char, len + 1> arr{};
            auto append = [i = 0, &arr](const auto& s) mutable {
               for (auto c : s) arr[i++] = c;
            };
            (append(Strs), ...);
            arr[len] = 0;
            return arr;
         }

         static constexpr auto arr = impl();  // Give the joined string static storage
         static constexpr std::string_view value{arr.data(), arr.size() - 1};
      };
// Helper to get the value out
#ifdef _MSC_VER
      template <svw... Strs>
#else
      template <const std::string_view&... Strs>
#endif
      static constexpr auto join_v = join<Strs...>::value;
   }

   /*template <class T>
   concept has_glaze_name = requires {
       T::glaze_name;
   };*/
}

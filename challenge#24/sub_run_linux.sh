#!/usr/bin/env bash
set -euo pipefail

# Work from the directory containing the script, even if called elsewhere.
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

results=results.txt
inputs=(
    poker_hands.json
    SOC24_SAMPLE_100000_hands.json
    SOC24_ALL_2598960_hands.json
)

# The Windows script must have created the file first. Never truncate it here.
if [[ ! -f "$results" ]]; then
    printf '[ERROR] %s does not exist. Run run_windows.cmd first.\n' "$results" >&2
    exit 1
fi

for compiler in gcc clang; do
    exe="./${compiler}/SOC24"
    if [[ ! -f "$exe" ]]; then
        printf '[ERROR] Missing executable: %s\n' "$exe" >&2
        exit 1
    fi

    for json in "${inputs[@]}"; do
        if [[ ! -f "$json" ]]; then
            printf '[ERROR] Missing input: %s\n' "$json" >&2
            exit 1
        fi

        printf '[%s] Running %s ...\n' "${compiler^^}" "$json"
        printf '\n==================== %s - %s ====================\n' "${compiler^^}" "$json" >> "$results"
        if "$exe" "$json" >> "$results" 2>&1; then
            printf '[%s] Completed %s\n' "${compiler^^}" "$json"
        else
            status=$?
            printf '[ERROR] %s failed on %s (exit %d). See %s.\n' "$exe" "$json" "$status" "$results" >&2
            exit "$status"
        fi
    done
done

printf '[Linux] All 6 runs completed. Output: %s\n' "$results"

printf '[Linux] Compressing: %s\n' "$results"
zip -9 -j results.zip results.txt
printf '[Linux] Done.'

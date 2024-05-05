#!/usr/bin/env python3

import json
import matplotlib.pyplot as plt
from shapely.geometry import shape
import matplotlib.patches as mpatches
from matplotlib.collections import PatchCollection

def plot_timezones(geojson_path):
    # Load the GeoJSON file
    with open(geojson_path, 'r') as f:
        data = json.load(f)
    
    # Prepare the plot
    fig, ax = plt.subplots(1, 1, figsize=(20, 10))
    patches = []
    for feature in data['features']:
        geometry = shape(feature['geometry'])
        if geometry.type == 'Polygon':
            polygon = mpatches.Polygon(list(geometry.exterior.coords), closed=True)
            patches.append(polygon)
        elif geometry.type == 'MultiPolygon':
            for poly in geometry.geoms:
                polygon = mpatches.Polygon(list(poly.exterior.coords), closed=True)
                patches.append(polygon)
    
    # Add patches to the axes
    p = PatchCollection(patches, cmap='tab20b', edgecolor='black')
    ax.add_collection(p)
    
    # Set plot title and labels
    ax.set_title('World Time Zones Map')
    ax.set_xlabel('Longitude')
    ax.set_ylabel('Latitude')
    
    # Remove x and y axis for a cleaner look
    ax.set_xticks([])
    ax.set_yticks([])

    # Show the plot
    plt.show()

# Replace 'path_to_timezones_geojson' with the path to your timezones GeoJSON file
plot_timezones('./timezones.json')
